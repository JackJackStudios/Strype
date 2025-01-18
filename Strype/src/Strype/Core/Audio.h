#pragma once

#include <glm/glm.hpp>

namespace Strype {

	class Audio
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetListenerPos(glm::vec2 pos);
	};

	class Sound
	{
	public:
		virtual ~Sound() = default;

		virtual void* GetNative() const = 0;
		static Ref<Sound> Create(const std::string& filepath);
	};

	class Source
	{
	public:
		virtual ~Source() = default;

		virtual void SetGain(float vol) = 0;
		virtual void SetLoop(bool loop) = 0;
		virtual void SetPitch(float pitch) = 0;
		virtual void SetPos(glm::vec2 pos) = 0;

		virtual void Play(Ref<Sound> sound) = 0;

		virtual void* GetNative() const = 0;
		static Ref<Source> Create();
	};

}
