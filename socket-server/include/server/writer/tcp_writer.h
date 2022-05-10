#pragma once
#include <server/writer.h>

namespace socket_server::writer {
    class TcpWriter : public socket_server::Writer {
    private:

    public:
        virtual err Write(const data& data) const override;
    };
}