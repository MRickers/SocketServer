#include <server/socket.h>
#include <server/socket/tcp_socket.h>
#include <server/error.h>

#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ifaddrs.h>

namespace socket_server {

    TcpSocket::TcpSocket() : fd_(-1), host_("") {
        fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(fd_ < 0) {
            throw ServerException("Invalid socket_fd", -1000);
        }
    }

    TcpSocket::TcpSocket(socket_fd fd) : fd_(fd), host_("") {
        if(fd_ < 0) {
            throw ServerException("Invalid socket_fd", -1000);
        }
    }

    Result<SocketBuffer> TcpSocket::Receive() const {
        return Result<SocketBuffer>(SocketBuffer());
    }

    err TcpSocket::Send(const SocketBuffer& buffer) const {
        return err::ok;
    }

    err ConnectTimeout(const std::string& host, uint16_t port, std::chrono::milliseconds timeout) {
        return err::ok;
    }

    err TcpSocket::Connect(const std::string& host, uint16_t port) {
        return err::ok;
    }

    err TcpSocket::Close() {
        return err::ok;
    }

    err TcpSocket::Bind(uint16_t port) {
        return err::ok;
    }

    err TcpSocket::Listen() {
        return err::ok;
    }

    Result<Client> TcpSocket::Accept() {
        return Result<Client>(Client());
    }
}