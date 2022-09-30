#include "Zmq_socket.hpp"

using namespace std::chrono_literals;
ZmqSocket_client::ZmqSocket_client(std::string ip, std::string port) {
    address = "tcp://" + ip + ":" + port;
    socket.connect(address);
}
ZmqSocket_client::~ZmqSocket_client() {
}
int ZmqSocket_client::send(std::string data) {
    socket.send(zmq::buffer(data), zmq::send_flags::none);
    return 0;
}
int ZmqSocket_client::recv(std::string& data) {
    zmq::message_t request;
    socket.recv(request, zmq::recv_flags::none);
    data = std::string(static_cast<char*>(request.data()), request.size());
    return 0;
}
ZmqSocket_server::ZmqSocket_server(std::string port) {
    address = "tcp://*:" + port;
    socket.bind(address);
}
ZmqSocket_server::~ZmqSocket_server() {
}
int ZmqSocket_server::send(std::string data) {
    socket.send(zmq::buffer(data), zmq::send_flags::none);
    return 0;
}
int ZmqSocket_server::recv(std::string& data) {
    zmq::message_t request;
    socket.recv(request, zmq::recv_flags::none);
    data = std::string(static_cast<char*>(request.data()), request.size());
    return 0;
}