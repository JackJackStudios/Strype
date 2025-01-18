#pragma once

#include "Strype/Core/Audio.h"

#include <AL/al.h>
#include <AL/alc.h>

namespace Strype {

	class OpenALSource : public Source
	{
	public:
		OpenALSource();
		virtual ~OpenALSource();

		virtual void SetGain(float vol);
		virtual void SetLoop(bool loop);
		virtual void SetPitch(float pitch);
		virtual void SetPos(glm::vec2 pos);

		virtual void Play(Ref<Sound> sound);

		virtual void* GetNative() const override { return (void*)m_RendererID; };

	private:
		ALuint m_RendererID = 0;
		ALuint m_BoundBuffer = 0;
	};

}
