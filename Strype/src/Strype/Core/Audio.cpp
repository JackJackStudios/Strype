#include "stypch.h"
#include "Strype/Core/Audio.h"

#ifdef STY_PLATFORM_WINDOWS
	#include "API/OpenAL/OpenALSound.h"
	#include "API/OpenAL/OpenALSource.h"
#endif

namespace Strype
{
	Ref<Sound> Sound::Create(const std::string& filepath)
	{
#ifdef STY_PLATFORM_WINDOWS
		return CreateScope<OpenALSound>(filepath);
#else
		STY_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

	Ref<Source> Source::Create()
	{
#ifdef STY_PLATFORM_WINDOWS
		return CreateScope<OpenALSource>();
#else
		STY_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

}
