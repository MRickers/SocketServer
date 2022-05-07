#pragma once
#include <server/error.h>
#include <server_constants.h>

namespace socket_server {
	using data = std::vector<uint8_t>;

	class SocketServerWriter {
	public:
		err Write(const data& data);
	};
}