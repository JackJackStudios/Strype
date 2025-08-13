#pragma once

#include "Strype/Project/Asset.hpp"
#include "Strype/Core/Buffer.hpp"

#include <glm/glm.hpp>
#include <miniaudio.h>

namespace Strype {

	class Audio
	{
	public:
		static void Init();
		static void Shutdown();

		static ma_engine* GetAudioEngine();
	};

	class AudioFile : public Asset
	{
	public:
		AudioFile(ma_decoder& decoder);
		~AudioFile();

		static AssetType GetStaticType() { return AssetType::AudioFile; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	private:
		ma_sound m_EngineID;
	};

}
