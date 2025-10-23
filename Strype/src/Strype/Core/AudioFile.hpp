#pragma once

#include "Strype/Project/Asset.hpp"
#include "Strype/Core/Buffer.hpp"

#include <glm/glm.hpp>
#include <miniaudio.h>

namespace Strype {

	class AudioFile : public Asset
	{
	public:
		AudioFile(const std::filesystem::path& filepath);
		~AudioFile();

		void Play();
		void PlayOn(const glm::vec2& position);

		static AssetType GetStaticType() { return AssetType::AudioFile; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	private:
		ma_decoder m_Decoder;
		ma_sound m_EngineID;
	};

}
