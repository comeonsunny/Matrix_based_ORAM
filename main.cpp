#include <iostream>
#include <cmath>
using TYPE_INDEX = unsigned long long;
#define BLOCK_SIZE 5  // in bytes
#define DB_SIZE 46   // in bytes
const double REAL_BLOCK_NUM = ceil((double)DB_SIZE / BLOCK_SIZE);
const TYPE_INDEX LENGTH_BLOCK_NUM = ceil(sqrt(REAL_BLOCK_NUM));
int main()
{
    std::cout << REAL_BLOCK_NUM << std::endl;
    // check the data type of REAL_BLOCK_NUM
    std::cout << LENGTH_BLOCK_NUM << std::endl;
    std::cout << (typeid(LENGTH_BLOCK_NUM) == typeid(TYPE_INDEX)) << std::endl;
    return 0;
}