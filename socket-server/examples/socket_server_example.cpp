#include <server/socket/tcp_socket.h>
#include <iostream>

int main() {
    using namespace socket_server;

    TcpSocket sock;

    if(sock.Bind(5000) != err::ok) {
        return -1;
    }

    if(sock.Listen() != err::ok) {
        return -2;
    }

    if(const auto connection = sock.Accept(); connection.error == err::ok) {
        std::cout << "Connected to client " << connection.data.host << " on port " << connection.data.port;

        TcpSocket new_sock(connection.data.fd);

        while(true) {
            if(const auto recv = new_sock.Receive(); recv.error == err::ok) {
                if(recv.data.size() == 0) {
                    new_sock.Close();
                    break;
                }
                std::cout << std::string(recv.data.cbegin(), recv.data.cend());
            } else {
                new_sock.Close();
                break;
            }
        }
    }

    sock.Close();

    return 0;
}