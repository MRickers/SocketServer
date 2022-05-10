#pragma once
#include <array>

namespace socket_server {
	const size_t server_response_buffer_size = 2048;

	using ResponseBuffer = std::array<uint8_t, server_response_buffer_size>;
}
