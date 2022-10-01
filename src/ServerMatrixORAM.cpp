#include "ServerMatrixORAM.hpp"
#include <iostream>
#include <fstream>
using namespace std;
ServerMatrixORAM::ServerMatrixORAM(TYPE_INDEX block_size, TYPE_INDEX db_size, TYPE_INDEX real_block_num, TYPE_INDEX length_block_num, TYPE_INDEX total_block_num){
    this->block_size = block_size;
    this->db_size = db_size;
    this->real_block_num = real_block_num;
    this->length_block_num = length_block_num;
    this->total_block_num = total_block_num;
    this->p = CURRENT_PATH;
    for (auto dir : DB_PATH_DIR) {
        this->p /= dir;
        if (!fs::exists(this->p)) {
            fs::create_directory(this->p);
        }
    }
}
ServerMatrixORAM::~ServerMatrixORAM(){
}
int ServerMatrixORAM::run(){
    std::cout<< "[Server] Socket is OPEN on << " << SERVER_IP << ":" << SERVER_PORT << std::endl;
    ZmqSocket_server zmq_server(SERVER_PORT);
    while (true) {
        std::cout << "[Server] Waiting for a command from client..." << std::endl;
        zmq_server.recv(command_recv);
        std::cout << "[Server] Received command: " << command_recv << std::endl;
        zmq_server.send(COMMAND_SUCCESS);
        if (command_recv == COMMAND_SEND_DB) {
            cout<<endl;
			cout << "=================================================================" << endl;
            cout<< "[Server] Receiving ORAM Data..." <<endl;
            cout << "=================================================================" << endl;
            recv_db_from_client(zmq_server);
            cout << "=================================================================" << endl;
            cout<< "[Server] Receiving ORAM Data...Done" <<endl;
            cout << "=================================================================" << endl;
            cout<<endl;
        }
    }
    return 0;
}
int ServerMatrixORAM::recv_db_from_client(ZmqSocket_server& zmq_server){
   // step1 : open the server.db to be ready store the database from the server
    std::ofstream ofs(p / "server.db", std::ios::binary);
    if (!ofs) {
        std::cerr << "[Server] Error: cannot open the file: " << p << std::endl;
        return -1;
    }
    // step2 : receive the database from the client
    for (TYPE_INDEX i = 0; i < this->total_block_num; ++i) {
        zmq_server.recv(buffer_in_str);
        ofs.seekp(i * this->block_size * sizeof(TYPE_DATA));
        ofs.write(buffer_in_str.c_str(), this->block_size * sizeof(TYPE_DATA));
        zmq_server.send(COMMAND_SUCCESS);
    }
    return 0;

}