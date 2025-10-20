#pragma once

#include "Base.hpp"
#include "Strype/Utils/LoggingHelpers.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Strype {

	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

		static void LogMessage(spdlog::level::level_enum logLevel, std::string_view tag, std::string_view msg)
		{
			s_CoreLogger->log(logLevel, "[{}] {}", tag, msg);
		}

		template <typename... Args>
		static void LogMessage(spdlog::level::level_enum logLevel, std::string_view tag, spdlog::format_string_t<Args...> fmt, Args&&... args)
		{
			LogMessage(logLevel, tag, fmt::format(fmt, std::forward<Args>(args)...));
		}
	private:
		inline static std::shared_ptr<spdlog::logger> s_CoreLogger;
		inline static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

#define STY_LOG_TRACE(...) ::Strype::Log::LogMessage(spdlog::level::trace, __VA_ARGS__)
#define STY_LOG_INFO(...) ::Strype::Log::LogMessage(spdlog::level::info, __VA_ARGS__)
#define STY_LOG_WARN(...) ::Strype::Log::LogMessage(spdlog::level::warn, __VA_ARGS__)
#define STY_LOG_ERROR(...) ::Strype::Log::LogMessage(spdlog::level::err, __VA_ARGS__)

#define STY_VERIFY(condition, ...) { if(!(condition)) { STY_LOG_ERROR("VERIFY", __VA_ARGS__); STY_DEBUGBREAK(); } }

/*
#define STY_CORE_TRACE(...)    ::Strype::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define STY_CORE_INFO(...)     ::Strype::Log::GetCoreLogger()->info(__VA_ARGS__)
#define STY_CORE_WARN(...)     ::Strype::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define STY_CORE_ERROR(...)    ::Strype::Log::GetCoreLogger()->error(__VA_ARGS__)

// Client log macros
#define STY_TRACE(...)         ::Strype::Log::GetClientLogger()->trace(__VA_ARGS__)
#define STY_INFO(...)          ::Strype::Log::GetClientLogger()->info(__VA_ARGS__)
#define STY_WARN(...)          ::Strype::Log::GetClientLogger()->warn(__VA_ARGS__)
#define STY_ERROR(...)         ::Strype::Log::GetClientLogger()->error(__VA_ARGS__)
*/