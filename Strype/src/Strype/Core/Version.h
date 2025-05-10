#pragma once

#define STY_VERSION "0.4"

//
// Build Configuration
//
#if defined(STY_DEBUG)
#define STY_BUILD_CONFIG_NAME "Debug"
#elif defined(STY_RELEASE)
#define STY_BUILD_CONFIG_NAME "Release"
#elif defined(STY_DIST)
#define STY_BUILD_CONFIG_NAME "Dist"
#else
#error Undefined configuration?
#endif

//
// Build Platform
//
#if defined(STY_PLATFORM_WINDOWS)
#define STY_BUILD_PLATFORM_NAME "Windows x64"
#elif defined(STY_PLATFORM_LINUX)
#define STY_BUILD_PLATFORM_NAME "Linux"
#else
#error Undefined Platform?
#endif

#define STY_VERSION_LONG "Strype " STY_VERSION " (" STY_BUILD_PLATFORM_NAME " " STY_BUILD_CONFIG_NAME ")"
