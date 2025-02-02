#include "stypch.h"
#include "API/OpenAL/OpenALSound.h"

namespace Strype {

	OpenALSound::OpenALSound(const std::string& filepath)
	{

		SF_INFO sfinfo;
		SNDFILE* sndfile = sf_open(filepath.c_str(), SFM_READ, &sfinfo);

		STY_CORE_ASSERT(sndfile, "Could not open sound file");
		STY_CORE_ASSERT((sfinfo.frames >= 1 && sfinfo.frames <= (sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels), "Bad sample count in sound file");
		STY_CORE_ASSERT(sfinfo.channels <= 4, "Unsupported channel count in sound file");

		ALenum format = AL_NONE;
		switch (sfinfo.channels)
		{
		case 1:
			format = AL_FORMAT_MONO16;
			break;
		case 2:
			format = AL_FORMAT_STEREO16;
			break;
		case 3:
			if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
				format = AL_FORMAT_BFORMAT2D_16;
			break;

		case 4:
			if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
				format = AL_FORMAT_BFORMAT3D_16;
			break;
		}

		short* membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

		sf_count_t num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
		ALsizei num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

		STY_CORE_ASSERT(num_frames >= 1, "Cannot read samples in sound file");

		ALuint buffer = 0;
		alGenBuffers(1, &buffer);
		alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

		free(membuf);
		sf_close(sndfile);

		STY_CORE_ASSERT(!alGetError(), "OpenAL could not read sound file");

		m_RendererID = buffer;
	}

	OpenALSound::~OpenALSound()
	{
		alDeleteBuffers(1, &m_RendererID);
	}

}
