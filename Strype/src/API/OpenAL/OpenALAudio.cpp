#include "stypch.h"
#include "Strype/Core/Audio.h"

#include <AL/al.h>
#include <AL/alc.h>

namespace Strype {

	struct AudioData
	{
		ALCdevice* m_Device = nullptr;
		ALCcontext* m_Context = nullptr;
	};

	static AudioData s_Data;

	void Audio::Init()
	{
		s_Data.m_Device = alcOpenDevice(nullptr);
		STY_CORE_ASSERT(s_Data.m_Device, "Could not find audio device");

		s_Data.m_Context = alcCreateContext(s_Data.m_Device, nullptr);
		STY_CORE_ASSERT(s_Data.m_Context, "Could not create OpenAL context");

		STY_CORE_ASSERT(alcMakeContextCurrent(s_Data.m_Context), "Could not load OpenAL context")

		const ALCchar* extensions = alcGetString(s_Data.m_Device, ALC_EXTENSIONS);
		const ALCchar* device = nullptr;

		if (alcIsExtensionPresent(s_Data.m_Device, "ALC_ENUMERATE_ALL_EXT"))
			device = alcGetString(s_Data.m_Device, ALC_ALL_DEVICES_SPECIFIER);

		if (!device || alcGetError(s_Data.m_Device) != AL_NO_ERROR)
			device = alcGetString(s_Data.m_Device, ALC_DEVICE_SPECIFIER);

		STY_CORE_INFO("OpenAL Info:");
		STY_CORE_INFO("   Device: {0}", device ? device : "Unknown");
		STY_CORE_INFO("   Extensions: {0}", extensions ? extensions : "None");
	}

	void Audio::Shutdown()
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(s_Data.m_Context);

		alcCloseDevice(s_Data.m_Device);

		STY_CORE_INFO("Shutting down OpenAL");
	}

	void Audio::SetListenerPos(glm::vec2 pos)
	{
		alListener3f(AL_POSITION, pos.x, pos.y, 0.0f);
	}

}
