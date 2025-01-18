#include "stypch.h"
#include "API/OpenAL/OpenALSource.h"

namespace Strype {
	
	OpenALSource::OpenALSource()
	{
		alGenSources(1, &m_RendererID);
		alSourcei(m_RendererID, AL_BUFFER, m_BoundBuffer);
	}

	void OpenALSource::Play(Ref<Sound> sound)
	{
		ALuint buffer = (ALuint)sound->GetNative();

		if (buffer != m_BoundBuffer)
		{
			m_BoundBuffer = buffer;
			alSourcei(m_RendererID, AL_BUFFER, (ALint)m_BoundBuffer);
		}

		alSourcePlay(m_RendererID);
	}

	OpenALSource::~OpenALSource()
	{
		alDeleteSources(1, &m_RendererID);
	}

	void OpenALSource::SetGain(float vol)
	{
		alSourcef(m_RendererID, AL_GAIN, vol);
	}

	void OpenALSource::SetLoop(bool loop)
	{
		alSourcei(m_RendererID, AL_LOOPING, loop);
	}

	void OpenALSource::SetPitch(float pitch)
	{
		alSourcef(m_RendererID, AL_PITCH, pitch);
	}

	void OpenALSource::SetPos(glm::vec2 pos)
	{
		alSource3f(m_RendererID, AL_POSITION, pos.x, pos.y, 0.0f);
	}

}
