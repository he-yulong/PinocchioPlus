#pragma once
#include "../../../vendor/spdlog-1.14.1/include/spdlog/spdlog.h"
#include "../../../vendor/spdlog-1.14.1/include/spdlog/sinks/stdout_color_sinks.h"
#include "../../../vendor/spdlog-1.14.1/include/spdlog/fmt/ostr.h"

namespace pp
{
	//enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };
	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<class spdlog::logger>& GetCoreLogger();
	private:
		static std::shared_ptr<class spdlog::logger> s_CoreLogger;
	};
}

// Core log macros
#define PP_CORE_TRACE(...)     ::pp::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PP_CORE_DEBUG(...)     ::pp::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define PP_CORE_INFO(...)      ::pp::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PP_CORE_WARN(...)      ::pp::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PP_CORE_ERROR(...)     ::pp::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PP_CORE_FATAL(...)     ::pp::Log::GetCoreLogger()->critical(__VA_ARGS__)

// TODO
//#define PP_Log(...)            ::dl::Log::Logging(__VA_ARGS__)