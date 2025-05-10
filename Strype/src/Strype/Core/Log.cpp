#include "stypch.h"
#include "Strype/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Strype {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");

#ifdef STY_DEBUG
		spdlog::level::level_enum loglevel = spdlog::level::trace;
#else
		spdlog::level::level_enum loglevel = spdlog::level::info;
#endif

		s_CoreLogger = std::make_shared<spdlog::logger>("STRYPE", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(loglevel);
		s_CoreLogger->flush_on(loglevel);

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_ClientLogger);
		s_ClientLogger->set_level(loglevel);
		s_ClientLogger->flush_on(loglevel);
	}

}
