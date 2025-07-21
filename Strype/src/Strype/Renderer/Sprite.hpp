#pragma once

#include "Strype/Asset/Asset.hpp"
#include "RenderPipeline.hpp"

#include <AGI/agi.hpp>

namespace Strype {

	class Sprite : public Asset
	{
	public:
		Sprite(AGI::Texture texture, int frames = 1);

		AGI::Texture& GetTexture() { return m_Texture; }
		int FrameCount() const { return m_FrameCount; }
		float GetFrameDelta() const { return 1 / m_FPS; }

		float GetFrameSize() const;
		TexCoords GetTexCoords(int frame = 0);

		static AssetType GetStaticType() { return AssetType::Sprite; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		bool operator==(const Sprite& other) const { return m_Texture == other.m_Texture; }
	private:
		AGI::Texture m_Texture;
		int m_FrameCount;

		float m_FPS = 60;
	};

}