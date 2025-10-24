#include "stypch.hpp"
#include "Strype/Core/Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Strype {

	static bool s_InitLogging = (Log::Init(), true);

	void Log::Init()
	{
#ifdef STY_DEBUG
		spdlog::level::level_enum loglevel = spdlog::level::trace;
#else
		spdlog::level::level_enum loglevel = spdlog::level::info;
#endif

		spdlog::sink_ptr console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console->set_pattern("%^[%T] %v%$");

		s_CoreLogger = std::make_shared<spdlog::logger>("STRYPE", console);
		s_CoreLogger->set_level(loglevel);
		s_CoreLogger->flush_on(loglevel);

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", console);
		s_ClientLogger->set_level(loglevel);
		s_ClientLogger->flush_on(loglevel);

		spdlog::register_logger(s_CoreLogger);
		spdlog::register_logger(s_ClientLogger);
	}

}
