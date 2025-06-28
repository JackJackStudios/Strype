#include "stypch.hpp"
#include "Audio.hpp"

#include <AL/al.h>
#include <AL/alc.h>

namespace Strype {

	AudioSource::AudioSource()
	{
		alGenSources(1, &m_RendererID);
		alSourcei(m_RendererID, AL_BUFFER, m_BoundBuffer);
	}

	AudioSource::~AudioSource()
	{
		alDeleteSources(1, &m_RendererID);
	}

	void AudioSource::Play(const Ref<AudioFile>& sound)
	{
		ALuint buffer = (ALuint)sound->m_RendererID;

		if (buffer != m_BoundBuffer)
		{
			m_BoundBuffer = buffer;
			alSourcei(m_RendererID, AL_BUFFER, (ALint)m_BoundBuffer);
		}

		alSourcePlay(m_RendererID);
	}

	void AudioSource::SetGain(float vol)
	{
		alSourcef(m_RendererID, AL_GAIN, vol);
	}

	void AudioSource::SetLoop(bool loop)
	{
		alSourcei(m_RendererID, AL_LOOPING, loop);
	}

	void AudioSource::SetPitch(float pitch)
	{
		alSourcef(m_RendererID, AL_PITCH, pitch);
	}

	void AudioSource::SetPos(const glm::vec2& pos)
	{
		alSource3f(m_RendererID, AL_POSITION, pos.x, pos.y, 0.0f);
	}

}