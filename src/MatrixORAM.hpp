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
    fs::path p_db;
    /* parameters about connection */
    std::string command_recv;
public:
    MatrixORAM(TYPE_INDEX block_size, TYPE_INDEX db_size, TYPE_INDEX real_block_num, TYPE_INDEX length_block_num, TYPE_INDEX total_block_num);
    ~MatrixORAM();
    int build_db(vector<TYPE_INDEX>& shuffle_id);
    int send_db_to_server();
    int access(TYPE_INDEX blockID, TYPE_INDEX index, TYPE_DATA* data, bool is_write);
    int test_initial_db(const std::string obj_str);
};
#endif