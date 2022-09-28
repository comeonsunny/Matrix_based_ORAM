#ifndef BLOCK_HPP
#define BLOCK_HPP
#include "Config.hpp"
#include <cstring>
class Block{
private:
    TYPE_INDEX blockID;
    TYPE_INDEX block_size;
public:
    Block(TYPE_INDEX blockID, TYPE_INDEX block_size);
    ~Block();
    char* data;
};

Block::Block(TYPE_INDEX blockID, TYPE_INDEX block_size){
    this->blockID = blockID;
    this->block_size = block_size * sizeof(TYPE_DATA);
    this->data = new char[block_size];
    memset(this->data, 0, block_size);
    // write blockID to data
    memcpy(this->data, &blockID, sizeof(TYPE_INDEX));
}

Block::~Block(){
}

#endif