#pragma once

#include <glm/glm.hpp>

namespace Strype {

	class Audio
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetListenerPos(const glm::vec2& pos);
	};

	class Sound
	{
	public:
		Sound(const std::filesystem::path& filepath);
		~Sound();

		uint32_t GetNative() const { return m_RendererID; }
	private:
		uint32_t m_RendererID = 0;
	};

	class Source
	{
	public:
		Source();
		~Source();

		void SetGain(float vol);
		void SetLoop(bool loop);
		void SetPitch(float pitch);
		void SetPos(glm::vec2 pos);

		void Play(Ref<Sound> sound);

		uint32_t GetNative() const { return m_RendererID; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_BoundBuffer = 0;
	};

}
