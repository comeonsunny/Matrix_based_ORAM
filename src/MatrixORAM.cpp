#include "MatrixORAM.hpp"
#include "Block.hpp"
#include "Config.hpp"
#include "Zmq_socket.hpp"
#include "progressbar.hpp"
#include <fstream>
#include <cassert>
#include <iostream>
MatrixORAM::MatrixORAM(TYPE_INDEX block_size, TYPE_INDEX db_size, TYPE_INDEX real_block_num, TYPE_INDEX length_block_num, TYPE_INDEX total_block_num){
    this->block_size = block_size;
    this->db_size = db_size;
    this->real_block_num = real_block_num;
    this->length_block_num = length_block_num;
    this->total_block_num = total_block_num;
    p_db = CURRENT_PATH;
    for (int i = 0; i < DB_PATH_DIR.size(); i++) {
        p_db /= DB_PATH_DIR[i];
        if (!fs::exists(p_db)) {
            fs::create_directory(p_db);
        }
    }
}
MatrixORAM::~MatrixORAM(){
}
int MatrixORAM::build_db(vector<TYPE_INDEX>& shuffle_id) {
    /* create a client_db.db file in the current path /Data/DataBase */
    std::fstream db_file;
    db_file.open(p_db / "client.db", std::ios::out | std::ios::binary);
    /* 1 create the real blocks and dummy blocks */
    std:cout << "==========================================================" << std::endl;
    std::cout << "1. Create encrypted real blocks and dummy blocks" << std::endl;
    progressbar bar(this->total_block_num);
    for (TYPE_INDEX i = 0; i < this->total_block_num; ++i) {
        Block* block = new Block(i, this->block_size);
        // encrypt the data in the block
        block->encrypt();
        /* 2 store block's data into specific position in the file based on its blockID */
        // // move the file pointer to the specific position
        db_file.seekp(shuffle_id[i] * (this->block_size * sizeof(TYPE_INDEX) + IV_SIZE), std::ios::beg);
        // first write the iv
        db_file.write((char*)block->get_iv(), IV_SIZE);
        // then write the data
        db_file.write(block->get_data(), this->block_size * sizeof(TYPE_DATA));
        // delete the block
        delete block;
        bar.update();
    }
    std::cout << std::endl;
    std::cout << "**********************************************************" << std::endl;
    db_file.close();
    return 0;
}
int MatrixORAM::send_db_to_server() {
    /* 1 create a instance of ZmqSocket_client */
    ZmqSocket_client zmq_client(SERVER_IP, SERVER_PORT);
    // 1.1 send the command COMMAND_SEND_DB to server
    zmq_client.send(COMMAND_SEND_DB);
    // 1.2 receive the command COMMAND_SUCCESS from server
    zmq_client.recv(command_recv);
    assert(command_recv == COMMAND_SUCCESS);
    /* 2 open the client_db.db file */
    std::fstream db_file;
    db_file.open(p_db / "client.db", std::ios::in | std::ios::binary);
    /* 3 send the file to the server */
    std::cout << "==========================================================" << std::endl;
    std::cout << "2. Send the encrypted real blocks and dummy blocks to server" << std::endl;
    progressbar bar_send(this->total_block_num);
    for (TYPE_INDEX i = 0; i < this->total_block_num; ++i) {
        /* 3.1 read the block's data from the file to the buffer_out */
        char* buffer_out = new char[this->block_size * sizeof(TYPE_DATA) + IV_SIZE];
        // db_file.seekg(i * (this->block_size * sizeof(TYPE_DATA) + IV_SIZE), ios::beg);
        db_file.read(buffer_out, this->block_size * sizeof(TYPE_DATA) + IV_SIZE);
        // /* TEST */
        // Block* block = new Block(0, this->block_size);
        // char* iv = new char[IV_SIZE];
        // memcpy(iv, buffer_out, IV_SIZE);
        // block->set_iv((unsigned char*)iv);
        // char* data = new char[this->block_size * sizeof(TYPE_DATA)];
        // memcpy(data, buffer_out + IV_SIZE, this->block_size * sizeof(TYPE_DATA));
        // block->set_data(data);
        // block->decrypt();
        // // get the block_id from the buffer_out
        // TYPE_INDEX block_id = *(TYPE_INDEX*)block->get_data();
        // std::cout << "send block_id: " << block_id << std::endl;
        // delete block;
        // delete[] iv;
        // delete[] data;
        // /* TEST */
        /* 3.2 send the buffer_out to the server */
        // convert the buffer_out to std::string
        std::string buffer_out_str(buffer_out, this->block_size * sizeof(TYPE_DATA));
        zmq_client.send(buffer_out_str);
        // receive the command COMMAND_SUCCESS from server
        zmq_client.recv(command_recv);
        assert(command_recv == COMMAND_SUCCESS);
        delete[] buffer_out;
        bar_send.update();
    }
    db_file.close();
    std::cout << std::endl;
    std::cout << "**********************************************************" << std::endl;
    return 0;
}
