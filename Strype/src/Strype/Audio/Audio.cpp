#include "stypch.hpp"
#include "Strype/Audio/Audio.hpp"

namespace Strype {

	static void AudioCallback(void* pUserData, ma_uint32 level, const char* pMessage)
	{
		switch (level)
		{
		case MA_LOG_LEVEL_DEBUG:   STY_CORE_TRACE(pMessage); break;
		case MA_LOG_LEVEL_INFO:    STY_CORE_INFO(pMessage); break;
		case MA_LOG_LEVEL_WARNING: STY_CORE_WARN(pMessage); break;
		case MA_LOG_LEVEL_ERROR:   STY_CORE_ERROR(pMessage); break;
		}
	}

	struct AudioData
	{
		ma_engine AudioEngine;
		ma_context Context;
	};

	static AudioData s_AudioData;

	void Audio::Init()
	{
		ma_result result = ma_engine_init(nullptr, &s_AudioData.AudioEngine);
		STY_CORE_VERIFY(result == MA_SUCCESS, "Failed to initialize audio engine");

		STY_CORE_INFO("Using audio device: {}", ma_engine_get_device(&s_AudioData.AudioEngine)->playback.name);
	}

	void Audio::Shutdown()
	{
		ma_engine_uninit(&s_AudioData.AudioEngine);
	}

	ma_engine* Audio::GetAudioEngine()
	{
		return &s_AudioData.AudioEngine;
	}

}
