#pragma once

namespace socket_server {
	enum class err {
		ok,

	};

	template<typename T>
	struct Result {
		Result(const T& data_, err error_=err::ok) : error(error_), data(data_) {}
		err error;
		T data;
	};
}