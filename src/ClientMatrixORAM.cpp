#include "ClientMatrixORAM.hpp"
#include "MatrixORAM.hpp"

#include <random>
#include <algorithm>
#include <iostream>
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

    return 0;
}

void ClientMatrixORAM::test() {
    // cout all the elements in the position_map
    for (TYPE_INDEX i = 0; i < position_map.size(); i++) {
        cout << "row_index: " << position_map[i].row_index << " col_index: " << position_map[i].col_index << endl;
    }
}