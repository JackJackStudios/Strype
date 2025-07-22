#include "stypch.hpp"
#include "Sprite.hpp"

#include "Renderer.hpp"

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

	Sprite::Sprite(AGI::Texture texture, int frames)
		: m_Texture(texture), m_FrameCount(frames)
	{
	}

	float Sprite::GetFrameSize() const
	{
		return m_Texture->GetWidth() / m_FrameCount;
	}

	TexCoords Sprite::GetTexCoords(int frame)
	{
		if (frame + 1 * GetFrameSize() > m_Texture->GetWidth())
		{
			STY_CORE_WARN("Frame goes out of bounds (Frame: {})", frame);
			return {};
		}

		if (m_FrameCount == 1)
			return RenderCaps::TextureCoords;

		return Utils::FlipTexCoords(Utils::BoxToTextureCoords({ frame * GetFrameSize(), 0.0f }, GetFrameSize(), m_Texture->GetHeight(), { m_Texture->GetWidth(), m_Texture->GetHeight() }));
	}

}