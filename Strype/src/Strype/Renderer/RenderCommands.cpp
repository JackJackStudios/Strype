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
	void Renderer::DrawSprite(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, Ref<Sprite> sprite, float frame)
	{
		glm::mat4 transform = GetTransform(position, size, rotation);
		if (sprite) transform *= glm::scale(glm::mat4(1.0f), glm::make_vec3(sprite->GetFrameSize()));

		TexCoords texCoords = sprite ? sprite->GetTexCoords(frame) : RenderCaps::TextureCoords;
		float slotIndex = sprite ? GetTextureSlot(sprite->GetTexture()) : 0.0f;

		DrawQuad(transform, colour, slotIndex, texCoords);
	}

	void Renderer::DrawText(const glm::vec3& position, const std::string& text, Ref<Font> font)
	{
		int pen_x = position.x;
		int pen_y = position.y; // baseline at position

		for (const auto& ch : text)
		{
			auto it = font->m_Glyphs.find(ch);

			FontGlyph& G = it->second;
			int glyph_x = pen_x + G.bearingX;
			int glyph_y = pen_y + (G.bearingY - G.height);

			glm::mat4 transform = GetTransform({ glyph_x, glyph_y, 0.0f }, { G.width, G.height }, 0.0f);
			TexCoords texcoords = { glm::vec2(G.u0, G.v0), glm::vec2(G.u1, G.v0), glm::vec2(G.u1, G.v1), glm::vec2(G.u0, G.v1) };

			for (size_t i = 0; i < 4; i++)
			{
				m_TextPipeline.SubmitAttribute("a_Position", transform * RenderCaps::VertexPositions[i]);
				m_TextPipeline.SubmitAttribute("a_Colour",   glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				m_TextPipeline.SubmitAttribute("a_TexCoord", Utils::FlipTexCoordsV(texcoords)[i]);
				m_TextPipeline.SubmitAttribute("a_TexIndex", GetTextureSlot(font->m_AtlasTexture));

				m_TextPipeline.NextPoint();
			}

			m_TextPipeline.IndexCount += 6;
			pen_x += G.advanceX;
		}
	}

}