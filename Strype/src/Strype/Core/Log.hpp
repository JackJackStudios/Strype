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
	private:
		inline static std::shared_ptr<spdlog::logger> s_CoreLogger;
		inline static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#ifdef STY_DEBUG
#	define STY_CORE_TRACE(...)    ::Strype::Log::GetCoreLogger()->trace(__VA_ARGS__)
#else
#	define STY_CORE_TRACE(...) 
#endif

#define STY_CORE_INFO(...)     ::Strype::Log::GetCoreLogger()->info(__VA_ARGS__)
#define STY_CORE_WARN(...)     ::Strype::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define STY_CORE_ERROR(...)    ::Strype::Log::GetCoreLogger()->error(__VA_ARGS__); STY_DEBUGBREAK()
								 
// Client log macros			 
#define STY_TRACE(...)         ::Strype::Log::GetClientLogger()->trace(__VA_ARGS__)
#define STY_INFO(...)          ::Strype::Log::GetClientLogger()->info(__VA_ARGS__)
#define STY_WARN(...)          ::Strype::Log::GetClientLogger()->warn(__VA_ARGS__)
#define STY_ERROR(...)         ::Strype::Log::GetClientLogger()->error(__VA_ARGS__)