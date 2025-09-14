#pragma once

#include "Strype/Project/Asset.hpp"
#include "RenderPipeline.hpp"

#include <AGI/agi.hpp>

namespace Strype {

	class Sprite : public Asset
	{
	public:
		Sprite(AGI::TextureSpecification spec, int frames = 1);
		~Sprite();

		const AGI::TextureSpecification& GetSpecs() { return m_Specification; }
		int GetFrameCount() const { return m_FrameCount; }
		float GetFrameDelta() const { return 1 / m_FPS; }

		glm::vec2 GetFrameSize() const;

		TexCoords GetTexCoords(float frame = 0);

		static AssetType GetStaticType() { return AssetType::Sprite; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	private:
		AGI::TextureSpecification m_Specification;
		int m_FrameCount;

		float m_FPS = 60;
	};

}