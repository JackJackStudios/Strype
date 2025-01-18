#pragma once

#include "Strype/Core/Audio.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL\alext.h>
#include <sndfile/sndfile.h>

namespace Strype {

	class OpenALSound : public Sound
	{
	public:
		OpenALSound(const std::string& filepath);
		virtual ~OpenALSound();

		virtual void* GetNative() const override { return (void*)m_RendererID; };

	private:
		ALuint m_RendererID = 0;
	};

}
