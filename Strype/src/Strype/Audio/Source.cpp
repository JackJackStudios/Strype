#include "stypch.h"
#include "Audio.h"

#include <AL/al.h>
#include <AL/alc.h>

namespace Strype {

	Source::Source()
	{
		alGenSources(1, &m_RendererID);
		alSourcei(m_RendererID, AL_BUFFER, m_BoundBuffer);
	}

	void Source::Play(const Ref<AudioFile>& sound)
	{
		ALuint buffer = (ALuint)sound->GetNative();

		if (buffer != m_BoundBuffer)
		{
			m_BoundBuffer = buffer;
			alSourcei(m_RendererID, AL_BUFFER, (ALint)m_BoundBuffer);
		}

		alSourcePlay(m_RendererID);
	}

	Source::~Source()
	{
		alDeleteSources(1, &m_RendererID);
	}

	void Source::SetGain(float vol)
	{
		alSourcef(m_RendererID, AL_GAIN, vol);
	}

	void Source::SetLoop(bool loop)
	{
		alSourcei(m_RendererID, AL_LOOPING, loop);
	}

	void Source::SetPitch(float pitch)
	{
		alSourcef(m_RendererID, AL_PITCH, pitch);
	}

	void Source::SetPos(const glm::vec2& pos)
	{
		alSource3f(m_RendererID, AL_POSITION, pos.x, pos.y, 0.0f);
	}

}