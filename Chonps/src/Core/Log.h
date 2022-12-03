#ifndef CHONPS_LOG_H
#define CHONPS_LOG_H

#include "Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Chonps
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

	void logInit();
}

// Core Log macros
#define CHONPS_CORE_TRACE(...) ::Chonps::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CHONPS_CORE_INFO(...) ::Chonps::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CHONPS_CORE_WARN(...) ::Chonps::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CHONPS_CORE_ERROR(...) ::Chonps::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CHONPS_CORE_CRITICAL(...) ::Chonps::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log macros
#define CHONPS_TRACE(...) ::Chonps::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CHONPS_INFO(...) ::Chonps::Log::GetClientLogger()->info(__VA_ARGS__)
#define CHONPS_WARN(...) ::Chonps::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CHONPS_ERROR(...) ::Chonps::Log::GetClientLogger()->error(__VA_ARGS__)
#define CHONPS_CRITICAL(...) ::Chonps::Log::GetClientLogger()->critical(__VA_ARGS__)

#endif

