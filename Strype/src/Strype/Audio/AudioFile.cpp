#include "stypch.h"
#include "Audio.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL\alext.h>
#include <sndfile/sndfile.h>

namespace Strype {

	AudioFile::AudioFile(uint64_t frames, uint32_t channels, uint32_t samplerate)
	{
		STY_CORE_VERIFY(frames >= 1, "Cannot read samples in sound file");
		STY_CORE_VERIFY((frames >= 1 && frames <= (sf_count_t)(INT_MAX / sizeof(short)) / channels), "Bad sample count in sound file");
		STY_CORE_VERIFY(channels <= 4, "Unsupported channel count in sound file");

		ALenum format = AL_NONE;
		switch (channels)
		{
		case 1:
			format = AL_FORMAT_MONO16;
			break;
		case 2:
			format = AL_FORMAT_STEREO16;
			break;
		case 3:
			format = AL_FORMAT_BFORMAT2D_16;
			break;
		case 4:
			format = AL_FORMAT_BFORMAT3D_16;
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

	void AudioFile::SetData(const Buffer& buff) const
	{
		alBufferData(m_RendererID, m_DataFormat, buff.Data, buff.Size, m_SampleRate);
	}

}