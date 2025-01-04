#pragma once

#include "Strype/Core/PlatformDetection.h"

#ifdef STY_DEBUG
	#if defined(STY_PLATFORM_WINDOWS)
		#define STY_DEBUGBREAK() __debugbreak()
	#elif defined(STY_PLATFORM_LINUX)
		#include <signal.h>
		#define STY_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define STY_ENABLE_ASSERTS
#else
	#define STY_DEBUGBREAK()
#endif

#define STY_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Strype {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

#include "Strype/Core/Log.h"
#include "Strype/Core/Assert.h"