#include <iostream>
#include <cmath>
#include <cstring>
#include <random>
#include "src/Config.hpp"
#include "src/ClientMatrixORAM.hpp"
#include "src/ServerMatrixORAM.hpp"
using namespace std;

// a main function with arguments
int main(int argc, char *argv[]) {
    int choice;
    //=== Parameters ===============================================
    static TYPE_INDEX BLOCK_SIZE;  // in bytes >= 8
    static TYPE_INDEX DB_SIZE;  // in bytes
    if (argc > 3) {
        DB_SIZE = atoi(argv[1]) * 1024ull * 1024ull;
        BLOCK_SIZE = atoi(argv[2]) * 1024ull;
        choice = atoi(argv[3]);
    }
    static TYPE_INDEX REAL_BLOCK_NUM = ceil((double)DB_SIZE / (BLOCK_SIZE * sizeof(TYPE_DATA)));
    static TYPE_INDEX LENGTH_BLOCK_NUM = ceil(sqrt(REAL_BLOCK_NUM));
    static TYPE_INDEX TOTAL_BLOCK_NUM = LENGTH_BLOCK_NUM * LENGTH_BLOCK_NUM;
    cout << "BLOCK_SIZE: " << BLOCK_SIZE << endl;
    cout << "DB_SIZE: " << DB_SIZE << endl;
    cout << "REAL_BLOCK_NUM: " << REAL_BLOCK_NUM << endl;
    cout << "LENGTH_BLOCK_NUM: " << LENGTH_BLOCK_NUM << endl;
    cout << "TOTAL_BLOCK_NUM: " << TOTAL_BLOCK_NUM << endl;
    if (choice == 1) {
        //=== Client ===============================================
        ClientMatrixORAM client(BLOCK_SIZE, DB_SIZE, REAL_BLOCK_NUM, LENGTH_BLOCK_NUM, TOTAL_BLOCK_NUM);
        cout << "===========================================================" << endl;
        cout << "[Main]Client is initializing..." << endl;
        cout << "===========================================================" << endl;
        client.initialize();
        // // client.test_initial_db("client.db");
        // std::cout << "Client test finished" << std::endl;
        // client.test_initial_db("server.db");
        TYPE_DATA* data = new TYPE_DATA[BLOCK_SIZE];
        bool is_row = true;
beginning:
        cout << "How many times do you want to access the block of interest: ";
        TYPE_INDEX times;
        cin >> times;
        cout << "===========================================================" << endl;
        cout << "[Main]Client is accessing randomly..." << endl;
        cout << "===========================================================" << endl << endl;
        for (TYPE_INDEX i = 0; i < times; ++i) {
            // Generate a random integer range from 0 to REAL_BLOCK_NUM - 1
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, REAL_BLOCK_NUM - 1);
            // TYPE_INDEX block_id = dis(gen);
            TYPE_INDEX block_id = 0;
            cout << "Accessing block " << block_id << endl;
            client.access(block_id, data, false, is_row);
            TYPE_INDEX index = *(TYPE_INDEX*)data;
            cout << "index: " << index << endl;
            if (index != block_id) {
                cout << "Error: index != i" << endl;
                break;
            }
            is_row = !is_row;
            cout << "===========================================================" << endl;
        }
        cout << "===========================================================" << endl;
        goto beginning;
    } else if (choice == 2) {
        //=== Server ===============================================
        ServerMatrixORAM server(BLOCK_SIZE, DB_SIZE, REAL_BLOCK_NUM, LENGTH_BLOCK_NUM, TOTAL_BLOCK_NUM);
        cout << "===========================================================" << endl;
        cout << "[Main]Server is running" << endl;
        cout << "===========================================================" << endl;
        server.run();
    }
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
