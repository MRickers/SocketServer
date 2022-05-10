#pragma once
#include <vector>
#include <server/error.h>
namespace socket_server {
	using Response = std::vector<uint8_t>;

	class Writer {
	public:
		virtual ~Writer() {}

		virtual err Write(const Response& data) const = 0;
	};
}