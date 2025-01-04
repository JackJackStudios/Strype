#pragma once

#include "Strype/Core/Base.h"
#include "Strype/Core/Log.h"

#ifdef STY_ENABLE_ASSERTS
	#define STY_ASSERT(x, ...) { if(!(x)) { STY_ERROR("Assertion Failed: {0}", __VA_ARGS__); STY_DEBUGBREAK(); } }
	#define STY_CORE_ASSERT(x, ...) { if(!(x)) { STY_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); STY_DEBUGBREAK(); } }
#else
	#define STY_ASSERT(...)
	#define STY_CORE_ASSERT(...)
#endif

