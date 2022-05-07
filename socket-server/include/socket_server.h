#pragma once
#include <functional>

namespace socket_server {
	class SocketServer {
		using callback = std::function<void()>;
	public:
		virtual void ListenAndAccept(uint32_t port, callback fn) = 0;
	};

	class TcpSocketServer : public SocketServer{
		
	private:
		message_buffer buffer_;
	public:
		virtual void ListenAndAccept(uint32_t port, callback fn) override;
	}
}