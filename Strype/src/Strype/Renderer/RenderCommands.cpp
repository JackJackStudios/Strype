#include "stypch.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	void Renderer::OnWindowResize(const glm::vec2& size)
	{
		m_RenderContext->SetViewport(0, 0, size.x, size.y);
	}

	void Renderer::BeginFrame()
	{
		m_RenderContext->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		m_RenderContext->EndFrame();
	}

	void Renderer::SetClearColour(const glm::vec3& colour)
	{
		m_RenderContext->SetClearColour({ colour.x, colour.y, colour.z, 1.0f });
	}

	void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& colour, float slotIndex, TexCoords texcoords)
	{
		for (size_t i = 0; i < 4; i++)
		{
			m_QuadPipeline.SubmitAttribute("a_Position", transform * RenderCaps::VertexPositions[i]);
			m_QuadPipeline.SubmitAttribute("a_Colour", colour);
			m_QuadPipeline.SubmitAttribute("a_TexCoord", texcoords[i]);
			m_QuadPipeline.SubmitAttribute("a_TexIndex", slotIndex);

			m_QuadPipeline.NextPoint();
		}

		m_QuadPipeline.IndexCount += 6;
	}

	// Simple functions
	void Renderer::DrawSprite(const glm::vec3& position, const glm::vec2& scale, float rotation, const glm::vec4& colour, Ref<Sprite> sprite, float frame, SpriteAlign alignment, TexCoords texcoords)
	{
		glm::vec2 imageSize = Utils::SizeFromTexcoords(texcoords, sprite->GetFrameSize());
		glm::mat4 transform = GetTransform(position, scale * imageSize, rotation, alignment);

		DrawQuad(transform, colour, GetTextureSlot(sprite->GetTexture()), sprite->GetTexCoords(frame, texcoords));
	}

	void Renderer::DrawRect(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& colour)
	{
		glm::mat4 transform = GetTransform(position, scale, 0.0f, SpriteAlign(HoriAlign::Left, VertAlign::Top));
		DrawQuad(transform, colour, 0.0f, RenderCaps::TextureCoords);
	}

	void Renderer::DrawText(const glm::vec3& position, const glm::vec4& colour, const std::string& text, Ref<Font> font)
	{
		int pen_x = position.x;
		int pen_y = position.y; // baseline at position

		for (const auto& ch : text)
		{
			FontGlyph& data = font->m_Glyphs.find(ch)->second;

			int glyph_x = pen_x + data.bearingX;
			int glyph_y = pen_y + (data.bearingY - data.height);

			glm::mat4 transform = GetTransform({ glyph_x, glyph_y, 0.0f }, { data.width, data.height }, 0.0f);
			TexCoords texcoords = { glm::vec2(data.u0, data.v0), glm::vec2(data.u1, data.v0), glm::vec2(data.u1, data.v1), glm::vec2(data.u0, data.v1) };

			DrawQuad(transform, colour, GetTextureSlot(font->m_AtlasTexture), Utils::FlipTexCoordsV(texcoords));
			pen_x += data.advanceX;
		}
	}

}