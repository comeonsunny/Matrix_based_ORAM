#include "ClientMatrixORAM.hpp"
#include "MatrixORAM.hpp"
#include "Config.hpp"

#include <random>
#include <algorithm>
#include <iostream>
#include <cassert>
using namespace std;
ClientMatrixORAM::ClientMatrixORAM(TYPE_INDEX block_size, TYPE_INDEX db_size, TYPE_INDEX real_block_num, TYPE_INDEX length_block_num, TYPE_INDEX total_block_num) {
    this->block_size = block_size;
    this->db_size = db_size;
    this->real_block_num = real_block_num;
    this->length_block_num = length_block_num;
    this->total_block_num = total_block_num;
}
ClientMatrixORAM::~ClientMatrixORAM() {
    // TODO
}
/**
 * Function Name: initialize
 *
 * Description: 
 *    Initialize the client-side Matrix ORAM's metadata and database.
 *    step 1: create the position map
 *    step 2: build real blocks and dummy blocks and store them in the disk
 *    step 3: send the blocks to the server
 * 
 * @return 0 if successful
 */ 
int ClientMatrixORAM::initialize() {
    // create the position map
    /* 1 fill up shuffle_id with its capacity this->total_block_num */
    for (TYPE_INDEX i = 0; i < this->total_block_num; i++) {
        this->shuffle_id.push_back(i);
    }
    /* 2 shuffle the shuffle_id */
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffle_id.begin(), shuffle_id.end(), g);
    /* 3 create the position map */
    for (TYPE_INDEX i = 0; i < this->real_block_num; i++) {
        TYPE_POS_MAP pos_map;
        pos_map.row_index = shuffle_id[i] / this->length_block_num;
        pos_map.col_index = shuffle_id[i] % this->length_block_num;
        position_map.push_back(pos_map);
    }
    // build real blocks and dummy blocks and store them in the disk in the name of client_db.db
    MatrixORAM matrix_oram(this->block_size, this->db_size, this->real_block_num, this->length_block_num, this->total_block_num);
    matrix_oram.build_db(this->shuffle_id); 
    // send the blocks to the server
    matrix_oram.send_db_to_server();   
    return 0;
}
/**
 * Function Name: access
 *
 * Description: 
 *    access the block with blockID
 *    if is_write is true, update the corresponding block with data
 *    if is_write is false, read the corresponding block and store it in data
 *    step 1: send the access request to the server and get the response and send the index of interested block's row or column to the server
 *    step 2: receive and decrypt the data blocks in a row or a column from the server
 *    step 3: generate a random integer range from 0 to length_block_num - 1 
 *    step 4: swap the block with blockID and the block with the random integer
 *    step 5: update the position map and re-encrypt the data blocks in a row or a column
 *    step 6: send the updated data blocks to the server
 *    step 7: receive the corresponding successful message from the server
 * 
 * @return 0 if successful
 */ 
int ClientMatrixORAM::access(TYPE_INDEX blockID, TYPE_DATA* data, bool is_write) {
    // gain the index of interested block's row or column in the position map
    TYPE_INDEX row_index, col_index;
    MatrixORAM matrix_oram(this->block_size, this->db_size, this->real_block_num, this->length_block_num, this->total_block_num);
    TYPE_INDEX swap_block_index;
    if (IS_ROW) {
        row_index = position_map[blockID].row_index;
        std::cout << "row_index: " << row_index << std::endl;
        std::cout << "col_index: " << position_map[blockID].col_index << std::endl;
        swap_block_index = matrix_oram.access(blockID, row_index, data, is_write);
        // update the position map
        /*check the range of swap_block_index*/
        std::cout << "swap_block_index: " << swap_block_index << std::endl;
        assert(swap_block_index >= 0 && swap_block_index <this->total_block_num);
        swap(position_map[blockID].col_index, position_map[swap_block_index].col_index); 
    } else {
        col_index = position_map[blockID].col_index;
        swap_block_index = matrix_oram.access(blockID, col_index, data, is_write);
        // update the position map
        /*check the range of swap_block_index*/
        assert(swap_block_index >= 0 && swap_block_index < this->total_block_num);
        swap(position_map[blockID].row_index, position_map[swap_block_index].row_index);
    }
    IS_ROW = !IS_ROW;
    return 0;
}

void ClientMatrixORAM::test() {
    // cout all the elements in the position_map
    for (TYPE_INDEX i = 0; i < position_map.size(); i++) {
        cout << "row_index: " << position_map[i].row_index << " col_index: " << position_map[i].col_index << endl;
    }
}
int ClientMatrixORAM::test_initial_db(const std::string object_name) {
    MatrixORAM matrix_oram(this->block_size, this->db_size, this->real_block_num, this->length_block_num, this->total_block_num);
    matrix_oram.test_initial_db(object_name);
    return 0;
}