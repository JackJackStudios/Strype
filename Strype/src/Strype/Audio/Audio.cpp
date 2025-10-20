#include "stypch.hpp"
#include "Strype/Audio/Audio.hpp"

namespace Strype {

	static void AudioCallback(void* pUserData, ma_uint32 level, const char* pMessage)
	{
		switch (level)
		{
		case MA_LOG_LEVEL_WARNING: STY_LOG_WARN("Audio", std::string_view(pMessage)); break;
		case MA_LOG_LEVEL_ERROR:   STY_LOG_ERROR("Audio", std::string_view(pMessage)); break;
		}
	}

	struct AudioData
	{
		ma_engine AudioEngine;
		ma_context Context;
	};

	static AudioData s_AudioData;

	bool Audio::Init()
	{
		ma_log logger;
		ma_log_init(NULL, &logger);
		ma_log_register_callback(&logger, (ma_log_callback)AudioCallback);

		ma_engine_config config = ma_engine_config_init();
		config.pLog = &logger;

		ma_result result = ma_engine_init(&config, &s_AudioData.AudioEngine);
		if (result != MA_SUCCESS) return false;

		STY_LOG_INFO("Audio", "Using audio device: {}", ma_engine_get_device(&s_AudioData.AudioEngine)->playback.name);
		STY_LOG_TRACE("Audio", "  Format: {}",          ma_get_format_name(ma_engine_get_device(&s_AudioData.AudioEngine)->playback.format));
		STY_LOG_TRACE("Audio", "  Channels: {}",        ma_engine_get_device(&s_AudioData.AudioEngine)->playback.channels);
		STY_LOG_TRACE("Audio", "  Sample Rate: {}",     ma_engine_get_device(&s_AudioData.AudioEngine)->sampleRate);
		return true;
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
