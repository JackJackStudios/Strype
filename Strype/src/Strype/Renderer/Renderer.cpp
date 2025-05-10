#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

namespace Strype {

	static void OnAGIMessage(std::string_view message, AGI::LogLevel level)
	{
		switch (level)
		{
		case AGI::LogLevel::Trace:
			STY_CORE_TRACE("{}", std::string(message));
			break;
		case AGI::LogLevel::Info:
			STY_CORE_INFO("{}", std::string(message));
			break;
		case AGI::LogLevel::Warning:
			STY_CORE_WARN("{}", std::string(message));
			break;
		case AGI::LogLevel::Error:
			STY_CORE_ERROR("{}", std::string(message));
			break;
		}
	}

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Colour;
		glm::vec2 TexCoord;
		float TexIndex;
	};

	struct RendererData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
		static const size_t QuadVertexCount = 4;

		static constexpr glm::vec2 TextureCoords[] = { { 0.0f,  0.0f }, { 1.0f,  0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		std::shared_ptr<AGI::VertexArray> QuadVertexArray;
		std::shared_ptr<AGI::VertexBuffer> QuadVertexBuffer;
		std::shared_ptr<AGI::Shader> TextureShader;
		std::shared_ptr<AGI::Texture> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::vector<std::shared_ptr<AGI::Texture>> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4];
	};

	static RendererData s_RenderData;

	void Renderer::Init()
	{
		AGI::RenderAPI::SetPrintCallback(OnAGIMessage);

		s_RenderAPI = AGI::RenderAPI::Create(AGI::BestAPI());
		s_RenderAPI->Init((AGIloadproc)glfwGetProcAddress);

		s_RenderData.TextureSlots.resize(RendererData::MaxTextureSlots);

		s_RenderData.QuadVertexArray = AGI::VertexArray::Create();

		s_RenderData.QuadVertexBuffer = AGI::VertexBuffer::Create(s_RenderData.MaxVertices * sizeof(QuadVertex));
		s_RenderData.QuadVertexBuffer->SetLayout({
			{ AGI::ShaderDataType::Float3, "a_Position" },
			{ AGI::ShaderDataType::Float4, "a_Colour" },
			{ AGI::ShaderDataType::Float2, "a_TexCoord" },
			{ AGI::ShaderDataType::Float, "a_TexIndex" },
		});
		s_RenderData.QuadVertexArray->AddVertexBuffer(s_RenderData.QuadVertexBuffer);

		s_RenderData.QuadVertexBufferBase = new QuadVertex[s_RenderData.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_RenderData.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_RenderData.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<AGI::IndexBuffer> quadIB = AGI::IndexBuffer::Create(quadIndices, s_RenderData.MaxIndices);
		s_RenderData.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_RenderData.WhiteTexture = AGI::Texture::Create(1, 1, 4);
		uint32_t whiteTextureData = 0xffffffff;
		s_RenderData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_RenderData.MaxTextureSlots];
		for (uint32_t i = 0; i < s_RenderData.MaxTextureSlots; i++)
			samplers[i] = i;

		s_RenderData.TextureShader = AGI::Shader::Create(Application::Get().GetConfig().ShaderPath);
		s_RenderData.TextureShader->Bind();
		s_RenderData.TextureShader->SetIntArray("u_Textures", samplers, s_RenderData.MaxTextureSlots);
		
		// Set all texture slots to 0
		s_RenderData.TextureSlots[0] = s_RenderData.WhiteTexture;

		s_RenderData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_RenderData.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_RenderData.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_RenderData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer::Shutdown()
	{
		delete[] s_RenderData.QuadVertexBufferBase;
	}
	
	void Renderer::BeginRoom(Camera& camera)
	{
		s_RenderData.TextureShader->Bind();
		s_RenderData.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		
		s_RenderData.QuadIndexCount = 0;
		s_RenderData.QuadVertexBufferPtr = s_RenderData.QuadVertexBufferBase;
		
		s_RenderData.TextureSlotIndex = 1;
	}

	void Renderer::EndRoom()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_RenderData.QuadVertexBufferPtr - (uint8_t*)s_RenderData.QuadVertexBufferBase);
		s_RenderData.QuadVertexBuffer->SetData(s_RenderData.QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer::Flush()
	{
		if (s_RenderData.QuadIndexCount == 0)
			return; // Nothing to draw

		// Bind textures
		for (uint32_t i = 0; i < s_RenderData.TextureSlotIndex; i++)
			s_RenderData.TextureSlots[i]->Bind(i);

		s_RenderAPI->DrawIndexed(s_RenderData.QuadVertexArray, s_RenderData.QuadIndexCount);
	}

	void Renderer::FlushAndReset()
	{
		EndRoom();

		s_RenderData.QuadIndexCount = 0;
		s_RenderData.QuadVertexBufferPtr = s_RenderData.QuadVertexBufferBase;

		s_RenderData.TextureSlotIndex = 1;
	}

	void Renderer::DrawBasicQuad(const glm::mat4& transform, const glm::vec4& colour, const glm::vec2 texcoords[], const Ref<AGI::Texture>& texture)
	{
		if (s_RenderData.QuadIndexCount >= RendererData::MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;

		if (texture)
		{ 
			for (uint32_t i = 1; i < s_RenderData.TextureSlotIndex; i++)
			{
				if (*s_RenderData.TextureSlots[i].get() == *texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				if (s_RenderData.TextureSlotIndex >= RendererData::MaxTextureSlots)
					FlushAndReset();

				textureIndex = (float)s_RenderData.TextureSlotIndex;
				s_RenderData.TextureSlots[s_RenderData.TextureSlotIndex] = texture;
				s_RenderData.TextureSlotIndex++;
			}

		}

		for (size_t i = 0; i < RendererData::QuadVertexCount; i++)
		{
			s_RenderData.QuadVertexBufferPtr->Position = transform * s_RenderData.QuadVertexPositions[i];
			s_RenderData.QuadVertexBufferPtr->Colour = colour;
			s_RenderData.QuadVertexBufferPtr->TexCoord = texcoords[i];
			s_RenderData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_RenderData.QuadVertexBufferPtr++;
		}

		s_RenderData.QuadIndexCount += 6;
	}

	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour, const Ref<AGI::Texture>& texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawBasicQuad(transform, colour, RendererData::TextureCoords, texture);
	}

	void Renderer::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour, const Ref<AGI::Texture>& texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawBasicQuad(transform, colour, RendererData::TextureCoords, texture);
	}

}