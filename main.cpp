#include <iostream>
#include <cmath>
#include <cstring>
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
/* test class of Zmq_socket */
// int main() {
//    cout << "Input 1 to test ClientZmqSocket, 2 to test ServerZmqSocket" << endl;
//     int choice;
//     cin >> choice;
//     string ip = "localhost";
//     string port = "5555";
//     if (choice == 1) {
//         ZmqSocket_client client(ip, port);
//         // initialize a string with 1024 bytes
//         string data(1024 * 1024 * 2, 'a');
//         for (int i = 0; i < 10; ++i) {
//             cout << "Sending " << data << "..." << i << " times" << endl;
//             client.send(data);
//             client.recv(data);
//             cout << "Received " << data << endl;
//         }
//     } else if (choice == 2) {
//         ZmqSocket_server server(port);
//         string data;
//         while (true) {
//             cout << "waiting for client" << endl;
//             server.recv(data);
//             cout << data << "the length of data is " << data.length() << endl;
//             string reply = "copy that";
//             server.send(reply);
//         }
//     }
//     return 0;
// }
