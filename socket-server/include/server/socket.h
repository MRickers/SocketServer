#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <server/error.h>

#if _WIN32

#elif __linux__
#include <server/socket/tcp_socket_linux.h>
#endif

namespace socket_server {
    using SocketBuffer = std::vector<uint8_t>;
    
    struct Client {
        std::string host{""};
        uint16_t port{0};
        socket_fd fd{-1};
    };

    class Socket {
    public:
        virtual ~Socket() {}

        virtual Result<SocketBuffer> Receive() const = 0;
        virtual Result<size_t> Send(const SocketBuffer& buffer) const = 0;
        virtual err Connect(const std::string& host, uint16_t port) = 0;
        virtual err ConnectTimeout(const std::string& host, uint16_t port, std::chrono::milliseconds timeout) = 0;
        virtual err Close() = 0;
        virtual err Bind(uint16_t port) = 0;
        virtual err Listen() = 0;
        virtual Result<Client> Accept() = 0;

    };
}