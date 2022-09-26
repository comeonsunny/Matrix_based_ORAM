#ifndef Config_hpp
#define Config_hpp
#include <cmath>

//=== DATA TYPE ===============================================
using TYPE_INDEX = unsigned long long;
using TYPE_DATA = long long int;
//=== Parameters ===============================================
#define BLOCK_SIZE 16  // in bytes
#define DB_SIZE 1024   // in bytes
const TYPE_INDEX REAL_BLOCK_NUM = ceil((double)DB_SIZE / BLOCK_SIZE);
const TYPE_INDEX LENGTH_BLOCK_NUM = ceil(sqrt(REAL_BLOCK_NUM));
const TYPE_INDEX TOTAL_BLOCK_NUM = LENGTH_BLOCK_NUM * LENGTH_BLOCK_NUM;
const 
#endif