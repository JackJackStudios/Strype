#include "stypch.hpp"
#include "Strype/Audio/Audio.hpp"

#include <AL/al.h>
#include <AL/alc.h>

namespace Strype {

	struct AudioData
	{
		ALCdevice* m_Device = nullptr;
		ALCcontext* m_Context = nullptr;
	};

	static AudioData s_AudioData;

	void Audio::Init()
	{
		s_AudioData.m_Device = alcOpenDevice(nullptr);
		STY_CORE_VERIFY(s_AudioData.m_Device, "Could not find audio device");

		s_AudioData.m_Context = alcCreateContext(s_AudioData.m_Device, nullptr);
		STY_CORE_VERIFY(s_AudioData.m_Context, "Could not create  context");

		STY_CORE_VERIFY(alcMakeContextCurrent(s_AudioData.m_Context), "Could not load audio context")

		const ALCchar* extensions = alcGetString(s_AudioData.m_Device, ALC_EXTENSIONS);
		const ALCchar* device = nullptr;

		if (alcIsExtensionPresent(s_AudioData.m_Device, "ALC_ENUMERATE_ALL_EXT"))
			device = alcGetString(s_AudioData.m_Device, ALC_ALL_DEVICES_SPECIFIER);

		if (!device || alcGetError(s_AudioData.m_Device) != AL_NO_ERROR)
			device = alcGetString(s_AudioData.m_Device, ALC_DEVICE_SPECIFIER);

		STY_CORE_INFO("OpenAL Info:");
		STY_CORE_INFO("   Device: {0}", device ? device : "Unknown");
		STY_CORE_INFO("   Extensions: {0}", extensions ? extensions : "None");
	}

	void Audio::Shutdown()
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(s_AudioData.m_Context);

		alcCloseDevice(s_AudioData.m_Device);
	}

	void Audio::SetListenerPos(const glm::vec2& pos)
	{
		alListener3f(AL_POSITION, pos.x, pos.y, 0.0f);
	}

}
