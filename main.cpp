#include <iostream>
#include <cmath>
#include "src/Config.hpp"
#include "src/ClientMatrixORAM.hpp"
using namespace std;

// a main function with arguments
int main(int argc, char *argv[]) {
    //=== Parameters ===============================================
    static TYPE_INDEX BLOCK_SIZE;  // in bytes >= 8
    static TYPE_INDEX DB_SIZE;  // in bytes
    if (argc > 2) {
        BLOCK_SIZE = atoi(argv[1]) * 1024ull;
        DB_SIZE = atoi(argv[2]) * 1024ull;
    }
    static TYPE_INDEX REAL_BLOCK_NUM = ceil((double)DB_SIZE / BLOCK_SIZE);
    static TYPE_INDEX LENGTH_BLOCK_NUM = ceil(sqrt(REAL_BLOCK_NUM));
    static TYPE_INDEX TOTAL_BLOCK_NUM = LENGTH_BLOCK_NUM * LENGTH_BLOCK_NUM;
    ClientMatrixORAM client(BLOCK_SIZE, DB_SIZE, REAL_BLOCK_NUM, LENGTH_BLOCK_NUM, TOTAL_BLOCK_NUM);
    client.initialize();
    client.test();
    return 0;
}
