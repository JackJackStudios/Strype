#include "stypch.hpp"
#include "Sprite.hpp"

#include "Renderer.hpp"
#include <stb_image.h>

namespace Strype {

	namespace Utils {

		constexpr glm::vec2 PixelToTexcoords(const glm::vec2& pos, int atlas_width, int atlas_height)
		{
			return { pos.x / atlas_width, pos.y / atlas_height };
		}

		constexpr TexCoords BoxToTextureCoords(const glm::vec2& pos, int width, int height, const glm::vec2& atlas_size)
		{
			return { 
				PixelToTexcoords({ pos.x + width, pos.y + height }, atlas_size.x, atlas_size.y), 
				PixelToTexcoords({ pos.x, pos.y + height }, atlas_size.x, atlas_size.y),
				PixelToTexcoords(pos, atlas_size.x, atlas_size.y),
				PixelToTexcoords({ pos.x + width, pos.y }, atlas_size.x, atlas_size.y)
			};
		}

	};

	Sprite::Sprite(AGI::TextureSpecification spec, int frames)
		: m_Specification(spec), m_FrameCount(frames)
	{
	}

	Sprite::~Sprite()
	{
		stbi_image_free(m_Specification.Data);
	}

	glm::vec2 Sprite::GetFrameSize() const
	{
		return { (float)m_Specification.Width / m_FrameCount, m_Specification.Height };
	}

	TexCoords Sprite::GetTexCoords(float frame)
	{
		frame = std::floor(frame);
		if (frame + 1 * GetFrameSize().x > m_Specification.Width)
		{
			STY_CORE_WARN("Frame goes out of bounds (Frame: {})", frame);
			return {};
		}

		if (m_FrameCount == 1)
			return RenderCaps::TextureCoords;

		return Utils::FlipTexCoords(Utils::BoxToTextureCoords({ frame * GetFrameSize().x, 0.0f }, GetFrameSize().x, GetFrameSize().y, { m_Specification.Width, m_Specification.Height }));
	}

}