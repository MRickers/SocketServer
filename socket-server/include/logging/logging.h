#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>


namespace logging {
	enum class Loglevel : int { LEVEL_ALL = 0, LEVEL_INFO = 10, LEVEL_DEBUG = 30, LEVEL_WARN = 50, LEVEL_ERR = 70 };

	class Logger {
		static inline Loglevel log_level_ = Loglevel::LEVEL_ALL;
	private:
		static inline std::ostream* stream_ = &std::cout;
		static inline std::ofstream ostream_;
		std::stringstream sstream_;

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

		constexpr const char* convertLevel(Loglevel level) const {
			switch (level) {
			case Loglevel::LEVEL_DEBUG:
				return "[Debug]";
			case Loglevel::LEVEL_INFO:
				return "[Info]";
			case Loglevel::LEVEL_WARN:
				return "[Warn]";
			case Loglevel::LEVEL_ERR:
				return "[Error]";
			default:
				return "[All]";
			}
		}

		std::string formatLogging(const Loglevel level, const std::string& file, const int line) {
			std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
			std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(p.time_since_epoch());
			size_t ms_frac = ms.count() % 1000;
			std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);
			std::time_t t = s.count();

			std::stringstream ss{};
			ss << std::put_time(std::localtime(&t), "%F %T") << "." << ms_frac << " " << std::this_thread::get_id()
				<< " " <<  convertLevel(level) << " " << file_name(file.c_str()) << ":" << line << ": ";
			return ss.str();
		}

	public:
		Logger() :
			sstream_()
		{}

		~Logger() {
			*stream_ << sstream_.str() << std::endl << std::flush;
		}

		std::ostream& Get(const Loglevel level, const std::string& file, const uint32_t line) {
			sstream_ << formatLogging(level, file, line);
			return sstream_;
		} 

		static void SetLogLevel(const Loglevel level) {
			log_level_ = level;
		}

		static Loglevel GetLogLevel() {
			return log_level_;
		}

		static void SetStream(std::ostream& stream) {
			stream_ = &stream;
		}

		static void LogToFile(const std::string& filename) {
			ostream_.open(filename, std::ios::out | std::ios::app);
			stream_ = &ostream_;
		}
	};
}

#define lLog(level) if(level < logging::Logger::GetLogLevel());else logging::Logger().Get(level, __FILE__, __LINE__)

#define lAll	(logging::Loglevel::LEVEL_ALL)
#define lDebug	(logging::Loglevel::LEVEL_DEBUG)
#define lInfo	(logging::Loglevel::LEVEL_INFO)
#define lWarn	(logging::Loglevel::LEVEL_WARN)
#define lError	(logging::Loglevel::LEVEL_ERR)