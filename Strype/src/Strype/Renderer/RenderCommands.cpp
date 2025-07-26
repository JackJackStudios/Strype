#include "stypch.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	void Renderer::OnWindowResize(const glm::vec2& size)
	{
		m_RenderContext->SetViewport(0, 0, size.x, size.y);
	}

	void Renderer::Clear()
	{
		m_RenderContext->Clear();
	}

	void Renderer::SetClearColour(const glm::vec4& colour)
	{
		m_RenderContext->SetClearColour(colour);
	}

	// Simple functions
	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<Sprite>& sprite, const Buffer& buf)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (rotation != 0) transform = transform * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
		transform = transform * glm::scale(glm::mat4(1.0f), glm::make_vec3(size));

		auto texCoords = RenderCaps::TextureCoords;
		if (sprite) texCoords = sprite->GetTexCoords();

		for (size_t i = 0; i < 4; i++)
		{
			m_QuadPipeline.SubmitAttribute("a_Position", transform * RenderCaps::VertexPositions[i]);
			m_QuadPipeline.SubmitAttribute("a_Colour", colour);
			m_QuadPipeline.SubmitAttribute("a_TexCoord", texCoords[i]);
			m_QuadPipeline.SubmitAttribute("a_TexIndex", sprite ? GetTextureSlot(sprite->GetTexture()) : 0.0f);

			if (!m_QuadPipeline.UserAttribute.empty())
			{
				if (buf.Empty())
				{
					STY_CORE_WARN("Forgot to enter user data while drawing quad? \"{}\" ", m_QuadPipeline.UserAttribute);
				}
				else
				{
					m_QuadPipeline.SubmitAttribute(m_QuadPipeline.UserAttribute, buf);
				}
			}

			m_QuadPipeline.NextPoint();
		}

		m_QuadPipeline.IndexCount += 6;
	}

}