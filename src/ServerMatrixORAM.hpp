#ifndef ServerMatrixORAM_hpp
#define ServerMatrixORAM_hpp
#include "Config.hpp"
#include "Zmq_socket.hpp"
class ServerMatrixORAM  {
private:
    TYPE_INDEX block_size;
    TYPE_INDEX db_size;
    TYPE_INDEX real_block_num;
    TYPE_INDEX length_block_num;
    TYPE_INDEX total_block_num;
    /* parameters about COMMAND */
    std::string command_recv;
    /* parameters directory */
    fs::path p;
    /* parameters about data buffer */
    std::string buffer_in_str;
public:
    ServerMatrixORAM(TYPE_INDEX block_size, TYPE_INDEX db_size, TYPE_INDEX real_block_num, TYPE_INDEX length_block_num, TYPE_INDEX total_block_num);
    ~ServerMatrixORAM();
    int run();
    int recv_db_from_client(ZmqSocket_server& zmq_server);
};

#endif