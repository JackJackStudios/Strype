#pragma once

#include <glm/glm.hpp>

#include "Strype/Asset/Asset.h"

namespace Strype {

	class Audio
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetListenerPos(const glm::vec2& pos);
	};

	class AudioFile : public Asset
	{
	public:
		AudioFile(uint64_t frames, uint32_t channels, uint32_t samplerate);
		~AudioFile();

		static AssetType GetStaticType() { return AssetType::AudioFile; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		void SetData(const Buffer& buff) const;
		uint32_t GetNative() const { return m_RendererID; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_SampleRate;
		int m_DataFormat;
	};

	class Source
	{
	public:
		Source();
		~Source();

		void SetGain(float vol);
		void SetLoop(bool loop);
		void SetPitch(float pitch);
		void SetPos(const glm::vec2& pos);

		void Play(const Ref<AudioFile>& sound);

		uint32_t GetNative() const { return m_RendererID; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_BoundBuffer = 0;
	};

}
