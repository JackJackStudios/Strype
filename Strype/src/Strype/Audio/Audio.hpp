#pragma once

#include <glm/glm.hpp>

#include "Strype/Asset/Asset.hpp"

namespace Strype {

	class AudioFile : public Asset
	{
	public:
		AudioFile(uint64_t frames, uint32_t channels, uint32_t samplerate);
		~AudioFile();

		static AssetType GetStaticType() { return AssetType::AudioFile; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		void SetData(Buffer buf);
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_SampleRate;
		int m_DataFormat;

		friend class AudioSource;
	};

	class AudioSource
	{
	public:
		AudioSource();
		~AudioSource();

		void SetGain(float vol);
		void SetLoop(bool loop);
		void SetPitch(float pitch);
		void SetPos(const glm::vec2& pos);

		void Play(const Ref<AudioFile>& sound);
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_BoundBuffer = 0;
	};

	class Audio
	{
	public:
		static void Init();
		static void Shutdown();

		static Ref<AudioSource> GetGlobalSource() { return s_GlobalSource; }

		static void SetListenerPos(const glm::vec2& pos);
	private:
		inline static Ref<AudioSource> s_GlobalSource;
	};

}
