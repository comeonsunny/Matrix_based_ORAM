#ifndef ZMQ_SOCKET_HPP
#define ZMQ_SOCKET_HPP
#include <zmq.hpp>
#include <string>
#include <thread>
// create a class of ZmqSocket
class ZmqSocket_client {
private:
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
    std::string address;
public:
    ZmqSocket_client(std::string ip, std::string port);
    ~ZmqSocket_client();
    int send(std::string data);
    int recv(std::string& data);
};
class ZmqSocket_server {
private:
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::rep};
    std::string address;
public:
    ZmqSocket_server(std::string port);
    ~ZmqSocket_server();
    int send(std::string data);
    int recv(std::string& data);
};
#endif