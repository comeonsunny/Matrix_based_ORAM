#include "MatrixORAM.hpp"
#include "Block.hpp"
#include "Config.hpp"
#include "Zmq_socket.hpp"
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
    for (TYPE_INDEX i = 0; i < this->total_block_num; ++i) {
        Block block(i, this->block_size);
        /* 2 store block's data into specific position in the file based on its blockID */
        // move the file pointer to the specific position
        db_file.seekp(shuffle_id[i] * this->block_size * sizeof(TYPE_DATA), ios::beg);
        // write the block's data into the file
        db_file.write((char*)block.data, this->block_size * sizeof(TYPE_DATA));
    }
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
    for (TYPE_INDEX i = 0; i < this->total_block_num; ++i) {
        /* 3.1 read the block's data from the file to the buffer_out */
        char* buffer_out = new char[this->block_size * sizeof(TYPE_DATA)];
        db_file.seekg(i * this->block_size * sizeof(TYPE_DATA), ios::beg);
        db_file.read(buffer_out, this->block_size * sizeof(TYPE_DATA));
        // get the block_id from the buffer_out
        TYPE_INDEX block_id = *(TYPE_INDEX*)buffer_out;
        std::cout << "send block_id: " << block_id << std::endl;
        /* 3.2 send the buffer_out to the server */
        // convert the buffer_out to std::string
        std::string buffer_out_str(buffer_out, this->block_size * sizeof(TYPE_DATA));
        zmq_client.send(buffer_out_str);
        // receive the command COMMAND_SUCCESS from server
        zmq_client.recv(command_recv);
        assert(command_recv == COMMAND_SUCCESS);
    }
    db_file.close();

    return 0;
}