#include "MatrixORAM.hpp"
#include "Block.hpp"
#include "Config.hpp"

#include <fstream>
MatrixORAM::MatrixORAM(TYPE_INDEX block_size, TYPE_INDEX db_size, TYPE_INDEX real_block_num, TYPE_INDEX length_block_num, TYPE_INDEX total_block_num){
    this->block_size = block_size;
    this->db_size = db_size;
    this->real_block_num = real_block_num;
    this->length_block_num = length_block_num;
    this->total_block_num = total_block_num;
}
MatrixORAM::~MatrixORAM(){
}
int MatrixORAM::build_db(vector<TYPE_INDEX>& shuffle_id) {
    /* create a client_db.db file in the current path /Data/DataBase */
    fs::path p = CURRENT_PATH;
    for (int i = 0; i < DB_PATH_DIR.size(); i++) {
        p /= DB_PATH_DIR[i];
        if (!fs::exists(p)) {
            fs::create_directory(p);
        }
    }
    std::fstream db_file;
    db_file.open(p / "client.db", std::ios::out | std::ios::binary);
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
    return 0;
}