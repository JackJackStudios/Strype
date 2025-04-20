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
		AudioFile(const std::filesystem::path& filepath);
		~AudioFile();

		static AssetType GetStaticType() { return AssetType::AudioFile; }
		virtual AssetType GetType() const override { return GetStaticType(); }

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
		void SetPos(const glm::vec2& pos);

		void Play(const Ref<AudioFile>& sound);

		uint32_t GetNative() const { return m_RendererID; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_BoundBuffer = 0;
	};

}
