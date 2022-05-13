#include <server/socket/tcp_socket.h>

int main() {
    using namespace socket_server;
    TcpSocket sock;

    if(sock.Connect("localhost", 5000) != err::ok) {
        return -1;
    }

    const std::string message = "Hello there";

    if(const auto sent = sock.Send({message.cbegin(), message.cend()}); sent.error != err::ok) {
        return -1;
    }

    if(sock.Close() != err::ok) {
        return -1;
    }

    return 0;
}