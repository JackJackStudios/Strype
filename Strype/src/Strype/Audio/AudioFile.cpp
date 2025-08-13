#include "stypch.hpp"
#include "Audio.hpp"

namespace Strype {

	AudioFile::AudioFile(ma_decoder& decoder)
	{
		ma_sound_init_from_data_source(Audio::GetAudioEngine(), &decoder, MA_SOUND_FLAG_DECODE, nullptr, &m_EngineID);
	}

	AudioFile::~AudioFile()
	{
		ma_sound_uninit(&m_EngineID);
	}

}