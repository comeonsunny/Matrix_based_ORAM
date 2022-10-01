#ifndef MATRIXORAM_HPP
#define MATRIXORAM_HPP
#include "Config.hpp"
#include <vector>
using namespace std;
class MatrixORAM {
private:
    /* parameters about block number */
    TYPE_INDEX block_size;
    TYPE_INDEX db_size;
    TYPE_INDEX real_block_num;
    TYPE_INDEX length_block_num;
    TYPE_INDEX total_block_num;
    /* parameters about file path */
    fs::path p;
    /* parameters about connection */
    std::string command_recv;
public:
    MatrixORAM(TYPE_INDEX block_size, TYPE_INDEX db_size, TYPE_INDEX real_block_num, TYPE_INDEX length_block_num, TYPE_INDEX total_block_num);
    ~MatrixORAM();
    int build_db(vector<TYPE_INDEX>& shuffle_id);
    int send_db_to_server();
};
#endif