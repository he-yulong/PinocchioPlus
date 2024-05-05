#include "Log.h"
//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/stdout_color_sinks.h"
//#include "spdlog/fmt/ostr.h"

namespace pp
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T.%e] [%n] [%L]: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("PINOCCHIO_PlUS");
		s_CoreLogger->set_level(spdlog::level::trace);
	}

	std::shared_ptr<spdlog::logger>& Log::GetCoreLogger()
	{
		if (s_CoreLogger == nullptr) Init();
		return s_CoreLogger;
	}

}