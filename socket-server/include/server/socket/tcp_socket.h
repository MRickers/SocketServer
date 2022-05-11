#pragma once
#include <server/socket.h>

namespace socket_server {
    class TcpSocket : public Socket {
    private:
        socket_fd fd_ = -1;
        std::string host_ = "";
    public:
        TcpSocket();
        TcpSocket(socket_fd fd);
        
        virtual Result<SocketBuffer> Receive() const override;
        virtual err Send(const SocketBuffer& buffer) const override;
        virtual err Connect(const std::string& host, uint16_t port) override;
        virtual err Close() override;
        virtual err Bind(uint16_t port) override;
        virtual err Listen() override;
        virtual Result<Client> Accept() override;

        err ConnectTimeout(const std::string& host, uint16_t port, )

    };
}