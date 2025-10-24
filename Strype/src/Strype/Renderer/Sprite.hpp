#pragma once

#include "Strype/Project/Asset.hpp"
#include "RenderPipeline.hpp"

#include <AGI/agi.hpp>

namespace Strype {

	namespace Utils {

		AGI::Texture LoadTexture(const std::filesystem::path& path, bool linearFilter = false);

	};

	class Sprite : public Asset
	{
	public:
		Sprite(AGI::Texture texture, int frames = 1);
		~Sprite();

		const AGI::Texture& GetTexture() { return m_Texture; }
		const AGI::TextureSpecification& GetSpecs() const { return m_Texture->GetSpecification(); }

		int GetFrameCount() const { return m_FrameCount; }
		float GetFrameDelta(float ts) const { return ts * m_FPS; }

		glm::vec2 GetFrameSize() const;
		TexCoords GetTexCoords(float frame, const TexCoords& tex);

		static AssetType GetStaticType() { return AssetType::Sprite; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	private:
		AGI::Texture m_Texture;

		int m_FrameCount;
		float m_FPS = 5;
	};

}