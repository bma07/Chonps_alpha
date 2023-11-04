#ifndef HG_CHONPS_LOG_H
#define HG_CHONPS_LOG_H

#include "Config.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Chonps
{
	void logInit();

	spdlog::logger* getCoreLogger();
	spdlog::logger* getClientLogger();

	void setLoggerPattern(const std::string& pattern);
}

#define CHONPS_CORE_LOG(name, type, msg, ...)		::Chonps::getCoreLogger()->type(#name "-" #type ": " msg, ##__VA_ARGS__)
#define CHONPS_LOG(name, type, msg, ...)			::Chonps::getClientLogger()->type(#name "-" #type ": " msg, ##__VA_ARGS__)

#define CHONPS_FUNCTION_PROPERTIES(func, ...)		#func, fmt::ptr(&func), #__VA_ARGS__
#define CHONPS_VARIABLE_PROPERTIES(var, ...)		#var, fmt::ptr(&var), #__VA_ARGS__
#define CHONPS_OBJECT_PROPERTIES(obj, ...)			#obj, fmt::ptr(&obj), #__VA_ARGS__

// Core Log macros									
#define CHONPS_CORE_TRACE(...)						::Chonps::getCoreLogger()->trace(__VA_ARGS__)
#define CHONPS_CORE_INFO(...)						::Chonps::getCoreLogger()->info(__VA_ARGS__)
#define CHONPS_CORE_WARN(...)						::Chonps::getCoreLogger()->warn(__VA_ARGS__)
#define CHONPS_CORE_ERROR(...)						::Chonps::getCoreLogger()->error(__VA_ARGS__)
#define CHONPS_CORE_CRITICAL(...)					::Chonps::getCoreLogger()->critical(__VA_ARGS__)

#define CHONPS_CORE_LOG_TRACE(name, msg, ...)		::Chonps::getCoreLogger()->trace(#name "-trace: " msg, ##__VA_ARGS__)
#define CHONPS_CORE_LOG_INFO(name, msg, ...)		::Chonps::getCoreLogger()->info(#name "-info: " msg, ##__VA_ARGS__)
#define CHONPS_CORE_LOG_WARN(name, msg, ...)		::Chonps::getCoreLogger()->warn(#name "-warning: " msg, ##__VA_ARGS__)
#define CHONPS_CORE_LOG_ERROR(name, msg, ...)		::Chonps::getCoreLogger()->error(#name "-error: " msg, ##__VA_ARGS__)
#define CHONPS_CORE_LOG_CRITICAL(name, msg, ...)	::Chonps::getCoreLogger()->critical(#name "-critical: " msg, ##__VA_ARGS__)


// Client Log macros
#define CHONPS_TRACE(...)							::Chonps::getClientLogger()->trace(__VA_ARGS__)
#define CHONPS_INFO(...)							::Chonps::getClientLogger()->info(__VA_ARGS__)
#define CHONPS_WARN(...)							::Chonps::getClientLogger()->warn(__VA_ARGS__)
#define CHONPS_ERROR(...)							::Chonps::getClientLogger()->error(__VA_ARGS__)
#define CHONPS_CRITICAL(...)						::Chonps::getClientLogger()->critical(__VA_ARGS__)

#define CHONPS_LOG_TRACE(name, msg, ...)			::Chonps::getClientLogger()->trace(#name "-trace: " msg, ##__VA_ARGS__)
#define CHONPS_LOG_INFO(name, msg, ...)				::Chonps::getClientLogger()->info(#name "-info: " msg, ##__VA_ARGS__)
#define CHONPS_LOG_WARN(name, msg, ...)				::Chonps::getClientLogger()->warn(#name "-warning: " msg, ##__VA_ARGS__)
#define CHONPS_LOG_ERROR(name, msg, ...)			::Chonps::getClientLogger()->error(#name "-error: " msg, ##__VA_ARGS__)
#define CHONPS_LOG_CRITICAL(name, msg, ...)			::Chonps::getClientLogger()->critical(#name "-critical: " msg, ##__VA_ARGS__)

#endif

