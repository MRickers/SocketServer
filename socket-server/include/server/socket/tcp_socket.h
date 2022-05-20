#pragma once
#include <server/socket.h>

namespace socket_server {
    class TcpSocket : public Socket {
    private:
        socket_fd fd_ = -1;
        std::string host_ = "";
        uint16_t port_{0};
    public:
        TcpSocket();
        ~TcpSocket();
        TcpSocket(socket_fd fd);
        
        virtual Result<SocketBuffer> Receive() const override;
        virtual Result<size_t> Send(const SocketBuffer& buffer) const override;
        virtual err Connect(const std::string& host, uint16_t port) override;
        virtual err ConnectTimeout(const std::string& host, uint16_t port, std::chrono::milliseconds timeout) override;
        virtual err Close() override;
        virtual err Bind(uint16_t port) override;
        virtual err Listen() override;
        virtual Result<Client> Accept() override;
    };
}