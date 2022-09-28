# ifndef ClientMatrixORAM_hpp
# define ClientMatrixORAM_hpp
#include "Config.hpp"
#include <string>
#include <vector>
class ClientMatrixORAM {
private:
    std::vector<TYPE_POS_MAP> position_map;
    std::vector<TYPE_INDEX> shuffle_id;
    /* parameters about block number */
    TYPE_INDEX block_size;
    TYPE_INDEX db_size;
    TYPE_INDEX real_block_num;
    TYPE_INDEX length_block_num;
    TYPE_INDEX total_block_num;

public:
    ClientMatrixORAM(TYPE_INDEX block_size, TYPE_INDEX db_size, TYPE_INDEX real_block_num, TYPE_INDEX length_block_num, TYPE_INDEX total_block_num);
    ~ClientMatrixORAM();
    int initialize();
    int access(TYPE_INDEX blockID, std::string op, TYPE_DATA& data); 

    void test();
};
# endif