#include "MatrixORAM.hpp"
#include "Block.hpp"
#include "Config.hpp"
#include "Zmq_socket.hpp"
#include "progressbar.hpp"
#include <fstream>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
/*
@author: SunChangqi 
@data:2022/10/19
*/
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
        db_file.seekp(shuffle_id[i] * (this->block_size * sizeof(TYPE_DATA) + IV_SIZE), std::ios::beg);
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
    command_recv.clear();
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
        command_recv.clear();
        delete[] buffer_out;
        bar_send.update();
    }
    db_file.close();
    std::cout << std::endl;
    std::cout << "**********************************************************" << std::endl;
    return 0;
}
int MatrixORAM::test_initial_db(const std::string obj_str) {
    // open the obj_str file
    std::ifstream db_file;
    if (obj_str == "client.db") {
        db_file.open(p_db / "client.db", std::ios::in | std::ios::binary);
    } else if (obj_str == "server.db") {
        db_file.open(p_db / "server.db", std::ios::in | std::ios::binary);
    } else {
        std::cout << "Error: the obj_str is not correct!" << std::endl;
        return -1;
    }
    if (!db_file.is_open()) {
        std::cout << "open client.db failed" << std::endl;
        return -1;
    }
    for (TYPE_INDEX i = 0; i < this->total_block_num; ++i) {
        char* buffer_out = new char[this->block_size * sizeof(TYPE_DATA) + IV_SIZE];
        db_file.read(buffer_out, this->block_size * sizeof(TYPE_DATA) + IV_SIZE);
        Block* block = new Block(0, this->block_size);
        char* iv = new char[IV_SIZE];
        memcpy(iv, buffer_out, IV_SIZE);
        block->set_iv((unsigned char*)iv);
        char* data = new char[this->block_size * sizeof(TYPE_DATA)];
        memcpy(data, buffer_out + IV_SIZE, this->block_size * sizeof(TYPE_DATA));
        block->set_data(data);
        block->decrypt();
        // get the block_id from the buffer_out
        TYPE_INDEX block_id = *(TYPE_INDEX*)block->get_data();
        std::cout << "block_id: " << block_id << std::endl;
        delete block;
        delete[] iv;
        delete[] data;
        delete[] buffer_out;
    }
    db_file.close();
    return 0;
}
int MatrixORAM::access(TYPE_INDEX blockID, TYPE_INDEX index, TYPE_DATA* Data, bool is_write, bool is_row) {
    ZmqSocket_client zmq_client(SERVER_IP, SERVER_PORT);
    // send the command COMMAND_ACCESS to server and test the response COMMAND_SUCCESS
    zmq_client.send(COMMAND_ACCESS);
    zmq_client.recv(command_recv);
    assert(command_recv == COMMAND_SUCCESS);
    command_recv.clear();
    // send the index to server and test the response COMMAND_SUCCESS
    std::string index_str((char*)&index, sizeof(TYPE_INDEX));
    zmq_client.send(index_str);
    zmq_client.recv(command_recv);
    assert(command_recv == COMMAND_SUCCESS); 
    command_recv.clear();
    std::string is_row_str((char*)&is_row, sizeof(bool));
    std::cout << "is_row: " << is_row << std::endl;
    zmq_client.send(is_row_str);
    // receive a row or a column blocks from server and decrypt them and store them in the stash file
    std::string buffer_in;
    fs::path p_stash = p_db / "stash.temp";
    std::ofstream stash_file(p_stash, ios::out | ios::binary);
    Block* block = new Block(0, this->block_size);
    TYPE_INDEX block_id;
    TYPE_INDEX block_index;
    for (TYPE_INDEX i = 0; i < this->length_block_num; ++i) {
        zmq_client.recv(buffer_in);
        zmq_client.send(COMMAND_SUCCESS);
        // decrypt the data
        char* iv = new char[IV_SIZE];
        memcpy(iv, buffer_in.c_str(), IV_SIZE);
        block->set_iv((unsigned char*)iv);
        char* data = new char[this->block_size * sizeof(TYPE_DATA)];
        memcpy(data, buffer_in.c_str() + IV_SIZE, this->block_size * sizeof(TYPE_DATA));
        block->set_data(data);
        block->decrypt();
        // get the block_id from the decrypted data
        block_id = *(TYPE_INDEX*)block->get_data();
        std::cout << "[MatrixORAM]block_id: " << block_id << std::endl;
        if (block_id == blockID) {
            block_index = i;
            // get the data from the block
            memcpy((char*)Data, block->get_data(), this->block_size * sizeof(TYPE_DATA));
        }
        // write the data to the stash file
        stash_file.write(block->get_data(), this->block_size * sizeof(TYPE_DATA));
        delete[] iv;
        delete[] data;
    }
    stash_file.close();
    delete block;
    zmq_client.recv(command_recv);
    assert(command_recv == COMMAND_SUCCESS);
    command_recv.clear();
    // generate a random integer range from 0 to length_block_num - 1
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<> dis(0, this->length_block_num - 1);
    TYPE_INDEX random_int = dis(g);
    // if i != random_int, swap the block at index i and the block at index random_int
    fstream stash_file_swap(p_stash, ios::in | ios::out | ios::binary);
    TYPE_INDEX block_id_swap;
    if (block_index != random_int) {
        std::cout << "block_index: " << block_index << std::endl;
        std::cout << "random_int: " << random_int << std::endl;
        char* buffer_swap = new char[this->block_size * sizeof(TYPE_DATA)];
        // read the block at index random_int to the buffer_swap
        stash_file_swap.seekg(random_int * this->block_size * sizeof(TYPE_DATA), ios::beg);
        stash_file_swap.read(buffer_swap, this->block_size * sizeof(TYPE_DATA));
        block_id_swap = *(TYPE_INDEX*)buffer_swap;
        // write the block at random_int to the block at index block_index
        stash_file_swap.seekp(block_index * this->block_size * sizeof(TYPE_DATA), ios::beg);
        stash_file_swap.write(buffer_swap, this->block_size * sizeof(TYPE_DATA));
        // write the data to the block at index random_int
        stash_file_swap.seekp(random_int * this->block_size * sizeof(TYPE_DATA), ios::beg);
        stash_file_swap.write((char*)Data, this->block_size * sizeof(TYPE_DATA));
        delete[] buffer_swap;
    } else {
        block_id_swap = blockID;
    }
    stash_file_swap.close();
    // re-encrypt the blocks in the stash file and send them to the server
    ifstream stash_file_re(p_stash, ios::in | ios::binary);
    Block* block_re = new Block(0, this->block_size);
    for (TYPE_INDEX i = 0; i < this->length_block_num; ++i) {
        // read the block from the stash file
        char* data_re = new char[this->block_size * sizeof(TYPE_DATA)];
        stash_file_re.read(data_re, this->block_size * sizeof(TYPE_DATA));
        block_re->set_data(data_re);
        // encrypt the block
        block_re->encrypt();
        // send the block to the server
        char* buffer_out = new char[IV_SIZE + this->block_size * sizeof(TYPE_DATA)];
        memcpy(buffer_out, block_re->get_iv(), IV_SIZE);
        memcpy(buffer_out + IV_SIZE, block_re->get_data(), this->block_size * sizeof(TYPE_DATA));
        std::string buffer_out_str(buffer_out, IV_SIZE + this->block_size * sizeof(TYPE_DATA));
        zmq_client.send(buffer_out_str);
        zmq_client.recv(command_recv);
        assert(command_recv == COMMAND_SUCCESS);
        command_recv.clear();
        delete[] buffer_out;
        delete[] data_re;
    }
    stash_file_re.close();
    delete block_re;
    return block_id_swap;
}
