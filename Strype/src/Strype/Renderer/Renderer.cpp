#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

#include "Strype/Core/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <numeric>
#include <GLFW/glfw3.h>

namespace Strype {

	static RendererData s_Data;

	namespace Utils {

		static void* ShiftPtr(void* origin, uint32_t shift)
		{
			return (uint8_t*)origin + shift;
		}
	};

	static void OnAGIMessage(std::string_view message, AGI::LogLevel level)
	{
		using enum AGI::LogLevel;
		switch (level)
		{
			case Trace:   STY_CORE_TRACE("{}", std::string(message)); break;
			case Info:    STY_CORE_INFO("{}", std::string(message)); break;
			case Warning: STY_CORE_WARN("{}", std::string(message)); break;
			case Error:   STY_CORE_ERROR("{}", std::string(message)); break;
		}
	}

	void Renderer::Init()
	{
		auto& appConfig = Application::Get().GetConfig();
		s_Data.Layout = appConfig.RendererLayout;

		std::vector<std::string> requiredAttributes = { "a_Position", "a_Colour", "a_TexCoord", "a_TexIndex" };
		for (const auto& attr : requiredAttributes) 
			STY_CORE_VERIFY(s_Data.Layout.HasElement(attr), "Missing layout element: {}", attr);

		s_RenderAPI = AGI::RenderAPI::Create(
		{
			.PreferedAPI = AGI::APIType::Guess,
			.loaderfunc = (AGI::OpenGLloaderFn)glfwGetProcAddress,
			.messagefunc = OnAGIMessage,
		});

		s_Data.QuadVertexArray = AGI::VertexArray::Create();
		s_Data.QuadVertexBuffer = AGI::VertexBuffer::Create(RendererData::MaxVertices, s_Data.Layout);
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
		
		s_Data.QuadVertexBufferBase = malloc(s_Data.QuadVertexBuffer->GetSize());

		std::vector<uint32_t> quadIndices(RendererData::MaxIndices);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		s_Data.QuadVertexArray->SetIndexBuffer(AGI::IndexBuffer::Create(quadIndices.data(), s_Data.MaxIndices));

		s_Data.WhiteTexture = AGI::Texture::Create(1, 1, 4);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		std::array<int32_t, RendererData::MaxTextureSlots> samplers;
		std::iota(samplers.begin(), samplers.end(), 0);

		s_Data.TextureShader = AGI::Shader::Create(appConfig.ShaderPath);
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers.data(), s_Data.MaxTextureSlots);

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		for (const auto& attr : s_Data.Layout)
			s_Data.AttributeCache[attr.Name] = attr;
	}

	void Renderer::Shutdown()
	{
		free(s_Data.QuadVertexBufferBase);
	}
	
	void Renderer::BeginRoom(Camera& camera)
	{
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer::EndRoom()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer::Flush()
	{
		if (s_Data.QuadIndexCount == 0) return;

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
			s_Data.TextureSlots[i]->Bind(i);

		s_RenderAPI->DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	}

	void Renderer::FlushAndReset()
	{
		EndRoom();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

    void Renderer::DrawBasicQuad(const glm::mat4 &transform, const glm::vec4 &colour, const glm::vec2 texcoords[], const Ref<AGI::Texture> &texture)
    {
		if (s_Data.QuadIndexCount + 6 > RendererData::MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;
		if (texture)
		{ 
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i] == *texture)
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				if (s_Data.TextureSlotIndex >= RendererData::MaxTextureSlots)
					FlushAndReset();

				textureIndex = (float)(s_Data.TextureSlotIndex);
				s_Data.TextureSlots[s_Data.TextureSlotIndex++] = texture;
			}
		}

		for (size_t i = 0; i < RendererData::QuadVertexCount; i++)
		{
			auto base = s_Data.QuadVertexBufferPtr;

			SubmitAttribute("a_Position", transform * s_Data.QuadVertexPositions[i]);
			SubmitAttribute("a_Colour", colour);
			SubmitAttribute("a_TexCoord", texcoords[i]);
			SubmitAttribute("a_TexIndex", textureIndex);

			for (auto& [name, value] : s_Data.UserAttributes)
				std::memcpy(Utils::ShiftPtr(s_Data.QuadVertexBufferPtr, s_Data.AttributeCache[name].Offset), &value, s_Data.AttributeCache[name].Size);

			s_Data.QuadVertexBufferPtr = Utils::ShiftPtr(s_Data.QuadVertexBufferPtr, s_Data.QuadVertexBuffer->GetLayout().GetStride());
		}

		s_Data.QuadIndexCount += 6;
		s_Data.UserAttributes.clear();
	}

    void Renderer::SubmitAttribute(const std::string& name, const std::any& value)
    {
		s_Data.UserAttributes[name] = value;
    }

}