#include "MatrixORAM.hpp"
#include "Block.hpp"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
MatrixORAM::MatrixORAM(TYPE_INDEX block_size, TYPE_INDEX db_size, TYPE_INDEX real_block_num, TYPE_INDEX length_block_num, TYPE_INDEX total_block_num){
    this->block_size = block_size;
    this->db_size = db_size;
    this->real_block_num = real_block_num;
    this->length_block_num = length_block_num;
    this->total_block_num = total_block_num;
}
MatrixORAM::~MatrixORAM(){
}
int MatrixORAM::build_db(vector<TYPE_INDEX>& shuffle_id){
    // prepare the directory to store the blocks
    fs::path p = fs::current_path();
    fs::create_directory(p / "Data");
    fs::create_directory(p / "Data" / "DataBase");
    /* create a client_db.db file in the current path /Data/DataBase */
    std::fstream db_file;
    db_file.open(p / "Data" / "DataBase" / "client_db.db", std::ios::out | std::ios::binary);
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