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

	class ServerException : public std::exception {
	private:
		std::string msg_;

		constexpr const char* file_name(const char* path) {
			const char* file = path;
			while (*path) {
				#if _WIN32
				if (*path++ == '\\') {
					file = path;
				}
				#elif __linux__
				if (*path++ == '/') {
					file = path;
				}
				#endif
			}
			return file;
		}

	public:
		ServerException(const char* msg, const int16_t wcon_error = 0, const char* file="", const int16_t line=0) : msg_(msg) {
			msg_ += " WconError(" + std::to_string(wcon_error) + ")";
			msg_ += " in ";
			msg_ += file_name(file);
			msg_ += ":";
			msg_ += std::to_string(line);
		}

		virtual const char* what() const noexcept override {
			return msg_.c_str();
		}
	};
	#define ServerException( message, wcon_error )		ServerException(message, wcon_error, __FILE__, __LINE__)
}