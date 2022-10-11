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
    while (true) {
        ZmqSocket_server zmq_server(SERVER_PORT);
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
        } else if (command_recv == COMMAND_ACCESS) {
            cout<<endl;
            cout << "=================================================================" << endl;
            cout<< "[Server] Retrieving a row or a column from ORAM data..." <<endl;
            cout << "=================================================================" << endl;
            retrieve_row_or_column(zmq_server);
            cout << "=================================================================" << endl;
            cout<< "[Server] Retrieving a row or a column from ORAM data...Done" <<endl;
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
        // ofs.seekp(i * (this->block_size + sizeof(TYPE_INDEX) + IV_SIZE), std::ios::beg);
        ofs.write(buffer_in_str.c_str(), this->block_size * sizeof(TYPE_DATA) + IV_SIZE);
        zmq_server.send(COMMAND_SUCCESS);
    }
    ofs.close();
    return 0;
}
int ServerMatrixORAM::retrieve_row_or_column(ZmqSocket_server& zmq_server) {
    // clear the buffer
    buffer_in_str.clear();
    // step1 : receive the row or column index from the client
    zmq_server.recv(buffer_in_str);
    zmq_server.send(COMMAND_SUCCESS);
    std::string is_row_str;
    zmq_server.recv(is_row_str);
    bool is_row = *(bool*)is_row_str.c_str();
    std::cout << "[Server] is_row: " << is_row << std::endl;
    // step2 : check global variable of IS_ROW to know whether send the row or column to the client
    std::fstream fs(p / "server.db", std::ios::binary | std::ios::in | std::ios::out);
    if (is_row) {
        /*get the row_index from buffer_in_str*/
        TYPE_INDEX row_index = *(TYPE_INDEX*)buffer_in_str.c_str();
        std::cout << "[server]row_index: " << row_index << std::endl;
        buffer_in_str.clear();
        /*read row block indexed by row_index from server.db and send them to the client*/
        TYPE_INDEX start_index = row_index * this->length_block_num;
        fs.seekg(start_index * (this->block_size * sizeof(TYPE_DATA) + IV_SIZE), std::ios::beg);
        for (TYPE_INDEX i = 0; i < this->length_block_num; ++i) {
            char* buffer = new char[this->block_size * sizeof(TYPE_DATA) + IV_SIZE];
            fs.read(buffer, this->block_size * sizeof(TYPE_DATA) + IV_SIZE);
            // convert buffer to string
            buffer_in_str = std::string(buffer, this->block_size * sizeof(TYPE_DATA) + IV_SIZE);
            zmq_server.send(buffer_in_str);
            zmq_server.recv(command_recv);
            assert(command_recv == COMMAND_SUCCESS);
            command_recv.clear();
        }
        buffer_in_str.clear();
        zmq_server.send(COMMAND_SUCCESS);
        // step3 : receive the re-encrypted row from the client and store them in server.db
        fs.seekp(start_index * (this->block_size * sizeof(TYPE_DATA) + IV_SIZE), std::ios::beg);
        for (TYPE_INDEX i = 0; i < this->length_block_num; ++i) {
            std::string tmp_buffer;
            zmq_server.recv(tmp_buffer);
            zmq_server.send(COMMAND_SUCCESS);
            fs.write(tmp_buffer.c_str(), this->block_size * sizeof(TYPE_DATA) + IV_SIZE);
        }
    } else {
        /*get the column_index from buffer_in_str*/
        TYPE_INDEX column_index = *(TYPE_INDEX*)buffer_in_str.c_str();
        std::cout << "[server]column_index: " << column_index << std::endl;
        buffer_in_str.clear();
        /*read column block indexed by column_index from server.db and send them to the client*/
        for (TYPE_INDEX i = 0; i < this->length_block_num; ++i) {
            char* buffer = new char[this->block_size * sizeof(TYPE_DATA) + IV_SIZE];
            fs.seekg((column_index + i * this->length_block_num) * (this->block_size * sizeof(TYPE_DATA) + IV_SIZE), std::ios::beg);
            fs.read(buffer, this->block_size * sizeof(TYPE_DATA) + IV_SIZE);
            // convert buffer to string
            buffer_in_str = std::string(buffer, this->block_size * sizeof(TYPE_DATA) + IV_SIZE);
            zmq_server.send(buffer_in_str);
            zmq_server.recv(command_recv);
            assert(command_recv == COMMAND_SUCCESS);
            command_recv.clear();
        }
        buffer_in_str.clear();
        zmq_server.send(COMMAND_SUCCESS);
        // step3 : receive the re-encrypted column from the client and store them in server.db
        for (TYPE_INDEX i = 0; i < this->length_block_num; ++i) {
            std::string tmp_buffer;
            zmq_server.recv(tmp_buffer);
            zmq_server.send(COMMAND_SUCCESS);
            fs.seekp((column_index + i * this->length_block_num) * (this->block_size * sizeof(TYPE_DATA) + IV_SIZE), std::ios::beg);
            fs.write(tmp_buffer.c_str(), this->block_size * sizeof(TYPE_DATA) + IV_SIZE);
        }
    }
    fs.close(); 
    return 0;
}