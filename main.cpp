#include <iostream>
#include "src/Config.hpp"
using namespace std;

// a main function with arguments
int main(int argc, char *argv[]) {
    if (argc > 2) {
        BLOCK_SIZE = atoi(argv[1]);
        DB_SIZE = atoi(argv[2]);
    }
    cout << "BLOCK_SIZE: " << BLOCK_SIZE << endl;
    cout << "DB_SIZE: " << DB_SIZE << endl;
    return 0;
}
