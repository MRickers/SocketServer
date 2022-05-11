#pragma once
#include <functional>
#include <memory>
#include <server/writer.h>
#include <request.h>

namespace socket_server {
	class SocketServer {
		using ResponseWriter = std::unique_ptr<Writer>;
		using Handler = std::function<void(const ResponseWriter writer, Request request)>;

	private:
		Handler handler_;
	public:
		void ListenAndAccept(uint32_t port);
	};
}