#pragma once
#include <array>

namespace socket_server {
	const size_t server_message_buffer_size = 2048;

	using message_buffer = std::array<uint8_t, server_message_buffer_size>;
}
