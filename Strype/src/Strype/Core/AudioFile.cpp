#include "stypch.hpp"
#include "AudioFile.hpp"

#include "Strype/Core/Application.hpp"

namespace Strype {

	AudioFile::AudioFile(const std::filesystem::path& filepath)
	{
		ma_decoder_init_file(filepath.string().c_str(), nullptr, &m_Decoder);
		ma_engine* engine = Application::Get().GetAudioEngine();

		ma_sound_init_from_data_source(engine, &m_Decoder, MA_SOUND_FLAG_DECODE, nullptr, &m_EngineID);
	}

	AudioFile::~AudioFile()
	{
		ma_sound_uninit(&m_EngineID);
		ma_decoder_uninit(&m_Decoder);
	}

	void AudioFile::Play()
	{
		ma_sound_start(&m_EngineID);
	}

	void AudioFile::PlayOn(const glm::vec2& position)
	{
		ma_sound_set_position(&m_EngineID, position.x, position.y, 0.0f);
		ma_sound_start(&m_EngineID);
	}

}