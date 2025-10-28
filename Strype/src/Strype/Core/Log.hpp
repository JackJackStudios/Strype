#pragma once

#include "Base.hpp"
#include "Strype/Utils/LoggingHelpers.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/bundled/color.h>

namespace Strype {

	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

		static void LogMessage(spdlog::level::level_enum logLevel, std::string_view tag, std::string_view msg)
		{
			if (!s_CoreLogger) Init();
			s_CoreLogger->log(logLevel, "[{}] {}", tag, msg);
		}

		template <typename... Args>
		static void LogMessage(spdlog::level::level_enum logLevel, std::string_view tag, spdlog::format_string_t<Args...> fmt, Args&&... args)
		{
			LogMessage(logLevel, tag, fmt::format(fmt, std::forward<Args>(args)...));
		}

		template <typename T>
		static void LogMessage(spdlog::level::level_enum logLevel, std::string_view tag, T&& arg)
			requires (!std::is_convertible_v<T, spdlog::string_view_t>)
		{
			LogMessage(logLevel, tag, fmt::to_string(std::forward<T>(arg)));
		}
	private:
		inline static std::shared_ptr<spdlog::logger> s_CoreLogger = nullptr;
		inline static std::shared_ptr<spdlog::logger> s_ClientLogger = nullptr;
	};

}

#define STY_LOG_TRACE(...) ::Strype::Log::LogMessage(spdlog::level::trace, __VA_ARGS__)
#define STY_LOG_INFO(...) ::Strype::Log::LogMessage(spdlog::level::info, __VA_ARGS__)
#define STY_LOG_WARN(...) ::Strype::Log::LogMessage(spdlog::level::warn, __VA_ARGS__)
#define STY_LOG_ERROR(...) ::Strype::Log::LogMessage(spdlog::level::err, __VA_ARGS__)

#define STY_VERIFY(condition, ...) { if(!(condition)) { STY_LOG_ERROR("VERIFY", __VA_ARGS__); STY_DEBUGBREAK(); } }