#include "stypch.hpp"
#include "Audio.hpp"

#include <AL/al.h>
#include <sndfile.h>

namespace Strype {

	AudioFile::AudioFile(uint64_t frames, uint32_t channels, uint32_t samplerate)
	{
		STY_CORE_VERIFY(frames >= 1, "Cannot read samples in sound file");
		STY_CORE_VERIFY((frames <= (sf_count_t)(INT_MAX / sizeof(short)) / channels), "Bad sample count in sound file");
		STY_CORE_VERIFY(channels <= 2, "Unsupported channel count in sound file");

		ALenum format = AL_NONE;
		switch (channels)
		{
		case 1:
			format = AL_FORMAT_MONO16;
			break;
		case 2:
			format = AL_FORMAT_STEREO16;
			break;
		}

		//INFO: Entering empty membuf here shows scary error message!
		alGenBuffers(1, &m_RendererID);
		
		STY_CORE_VERIFY(!alGetError(), "Could not read sound file");

		m_DataFormat = format;
		m_SampleRate = samplerate;
	}

	AudioFile::~AudioFile()
	{
		alDeleteBuffers(1, &m_RendererID);
	}

	void AudioFile::SetData(void* data, uint64_t size) const
	{
		alBufferData(m_RendererID, m_DataFormat, data, size, m_SampleRate);
	}

}