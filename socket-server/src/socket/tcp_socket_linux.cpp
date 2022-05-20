#include <server/socket.h>
#include <server/socket/tcp_socket.h>
#include <server/error.h>
#include <logging/logging.h>

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
#include <string.h>

namespace socket_server {

    TcpSocket::TcpSocket() : fd_(-1), host_(""), port_(0) {
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

    TcpSocket::~TcpSocket() {
        Close();
    }

    Result<SocketBuffer> TcpSocket::Receive() const {
        const uint16_t receive_size = 0x8000;
        int16_t received = 0;
        SocketBuffer buffer;
        buffer.resize(receive_size);
        
        received = recv(fd_, buffer.data(), receive_size, 0);

        if(received == -1) {
            lLog(lError) << "ReceiveTcp error: " << errno << " - " << strerror(errno);
            return Result<SocketBuffer>{SocketBuffer(), err::tcp_receive};
        }
        buffer.resize(received);

        return Result<SocketBuffer>(buffer);
    }

    Result<size_t> TcpSocket::Send([[maybe_unused]]const SocketBuffer& buffer) const {
        int32_t sent=0;
        if((sent = send(fd_, buffer.data(), buffer.size(), 0)) < 0) {
            lLog(lError) << "SendTcp error: " << errno << " - " << strerror(errno);
            return Result<size_t>{0, err::tcp_send};
        }
        return Result<size_t>{static_cast<size_t>(sent)};
    }

    err TcpSocket::ConnectTimeout(const std::string& host, uint16_t port, std::chrono::milliseconds timeout) {
        struct addrinfo hints;
        struct addrinfo* result = NULL;
        struct addrinfo* rp = NULL;
        char portnumber_string[33];
        int ret, valopt = -1;
        struct timeval tv = {0x00, 0x00};
        long arg;
        fd_set myset;   // menge an file deskriptoren
        socklen_t lon;
        
        if(timeout.count() > 0) {
            tv.tv_usec += timeout.count() * 1000;
            tv.tv_sec += tv.tv_usec / 1000000;
            tv.tv_usec %= 1000000;
        }

        snprintf(portnumber_string, 33, "%d", port);

        // Struct-Definition siehe https://www.man7.org/linux/man-pages/man3/getaddrinfo.3.html
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET; // Nur IPv4
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = 0;
        hints.ai_protocol = IPPROTO_TCP;

        // Set non-blocking 
        // Get fd status
        if( (arg = fcntl(fd_, F_GETFL , NULL)) < 0) { 
            lLog(lError) << "ConnectTimeout error: fnctl " << errno  << " - " << strerror(errno);
            Close();
            return err::tcp_connect_timeout;
        }
        arg |= O_NONBLOCK; 
        if( fcntl(fd_, F_SETFL, arg) < 0) { 
            lLog(lError) << "ConnectTimeout error: fcntl " << " - " << strerror(errno);
            Close();
            return err::tcp_connect_timeout;
        }

        ret = getaddrinfo(host.c_str(), portnumber_string, &hints, &result);
        if (ret != 0) {
            lLog(lError) << "ConnectTimeout error: DNS failure: " << host << ": " << ret << " - " << strerror(errno);
            goto END;
        }

        for (rp = result; rp != NULL; rp = rp->ai_next) {
            if ((ret=connect(fd_, rp->ai_addr, rp->ai_addrlen)) != -1)
                break;                  /* Success */

            if(errno == EINPROGRESS) {
                do {
                    FD_ZERO(&myset);
                    FD_SET(fd_, &myset);   // socket zur Menge der beschreibbaren Deskriptoren hinzufügen
                    ret = select(fd_ + 1, NULL, &myset, NULL, (timeout.count() > 0 ? &tv : NULL));// select ist auf 1024 deskriptoren beschränkt, bei mehr = undefined behaviour
                    if(ret < 0 && errno != EINTR) {
                        lLog(lError) << "ConnectTimeout error: select " << errno  << " - " << strerror(errno);
                        goto END;
                    }else if(ret > 0) {
                        lon = sizeof(int);
                        // ist ein Fehler aufgetreten
                        if(getsockopt(fd_, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) { 
                            lLog(lError) << "ConnectTimeout error: getsockopt " << errno  << " - " << strerror(errno);
                            goto END;
                        }
                        // check the value returned
                        if(valopt) {
                            lLog(lError) << "ConnectTimeout error: delay connect " << valopt  << " - " << strerror(valopt);
                            goto END;
                        }

                        // Set to blocking mode again... 
                        if( (arg = fcntl(fd_, F_GETFL, NULL)) < 0) {
                            lLog(lError) << "ConnectTimeout error: fcntl " << errno  << " - " << strerror(errno);
                            goto END;
                        } 
                        arg &= (~O_NONBLOCK); 
                        if( fcntl(fd_, F_SETFL, arg) < 0) { 
                            lLog(lError) << "ConnectTimeout error: fcntl " << errno  << " - " << strerror(errno);
                            goto END;
                        } 

                        freeaddrinfo(result);
                        return err::ok;
                    }else {
                        lLog(lError) << "ConnectTimeout error: timeout in select";
                        goto END;
                    }
                } while(1);
            }else {
                lLog(lError) << "ConnectTimeout error: " << errno  << " - " << strerror(errno);
            }
        }
        END:
        Close();
        freeaddrinfo(result);
        return err::tcp_connect_timeout;
    }

    err TcpSocket::Connect(const std::string& host, uint16_t port) {
        return ConnectTimeout(host, port, std::chrono::milliseconds(30000));
    }

    err TcpSocket::Close() {
        if(close(fd_) == -1) {
            lLog(lError) << "Close error: " << errno << " - " << strerror(errno);
            return err::tcp_close;
        }
        return err::ok;
    }

    err TcpSocket::Bind(uint16_t port) {
        struct sockaddr_in server;
        int ret = 0;

        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(port);

        // set reuse
        const int y = 1;
        if((ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y))) != 0) {
            lLog(lError) << "SetSocketReuse error: " << errno << " - " << strerror(errno);
            return err::tcp_bind;
        }

        if((ret=bind(fd_, (struct sockaddr*)&server, sizeof(server))) < 0) {
            lLog(lError) << "Bind error: Port " << port << " " << errno << " " << strerror(errno);
            Close();
            return err::tcp_bind;
        }
        port_ = port;
        return err::ok;
    }

    err TcpSocket::Listen() {
        if(listen(fd_, 5) == -1) {
            lLog(lError) << "Listen error:" << errno << "  - " << strerror(errno);
            return err::tcp_listen;
        }
        lLog(lInfo) << "Listening " << "on port " << port_;
        return err::ok;
    }

    Result<Client> TcpSocket::Accept() {
        socket_fd sock2;
        struct sockaddr_in client;
        socklen_t len;
        char buffer[64];

        len = sizeof(client);
        sock2 = accept(fd_, (struct sockaddr*)&client, &len);
        if(sock2 == -1) {
            lLog(lError) << "Accept error: " << errno << " " << strerror(errno);
            return Result<Client>(Client(), err::tcp_accept);
        }
        if(inet_ntop(AF_INET, &client.sin_addr, buffer, 64) == NULL) {
            lLog(lWarn) << "Accept error: could not determine client address " << errno << " " << strerror(errno);
            return Result<Client>(Client(), err::tcp_accept);
        }
        return Result<Client>(Client{std::string(buffer), client.sin_port, sock2});
    }
}