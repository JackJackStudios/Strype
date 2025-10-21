#include "stypch.hpp"
#include "Sprite.hpp"

#include "Renderer.hpp"
#include <stb_image.h>

namespace Strype {

	Sprite::Sprite(AGI::Texture texture, int frames)
		: m_Texture(texture), m_FrameCount(frames)
	{
	}

	Sprite::~Sprite()
	{
	}

	glm::vec2 Sprite::GetFrameSize() const
	{
		return { (float)GetSpecs().Size.x / m_FrameCount, GetSpecs().Size.y};
	}

	TexCoords Sprite::GetTexCoords(float frame, const TexCoords& tex)
	{
		frame = std::floor(frame);
		if (frame + 1 > m_FrameCount)
		{
			STY_LOG_WARN("Renderer", "Frame goes out of bounds (Frame: {})", frame);
			return {};
		}

		if (m_FrameCount == 1)
			return RenderCaps::TextureCoords;

		if (tex == RenderCaps::TextureCoords)
			return Utils::BoxToTextureCoords({ frame * GetFrameSize().x, 0.0f }, GetFrameSize().x, GetFrameSize().y, GetSpecs().Size);

		glm::vec2 atlasSize = Utils::SizeFromTexcoords(tex, GetFrameSize());
		glm::vec2 atlasPos = { tex[0].x * GetFrameSize().x, tex[0].y * GetFrameSize().y };
		return Utils::BoxToTextureCoords(atlasPos, atlasSize.x, atlasSize.y, GetSpecs().Size);
	}

}