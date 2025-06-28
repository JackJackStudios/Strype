#include "stypch.hpp"
#include "Strype/Renderer/Renderer.hpp"

#include "Strype/Core/Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <numeric>
#include <GLFW/glfw3.h>

namespace Strype {

	static void OnAGIMessage(std::string_view message, AGI::LogLevel level)
	{
		switch (level)
		{
			case AGI::LogLevel::Trace:   STY_CORE_TRACE("{}", std::string(message)); break;
			case AGI::LogLevel::Info:    STY_CORE_INFO("{}", std::string(message)); break;
			case AGI::LogLevel::Warning: STY_CORE_WARN("{}", std::string(message)); break;
			case AGI::LogLevel::Error:   STY_CORE_ERROR("{}", std::string(message)); break;
		}
	}

	void Renderer::Init()
	{
		AGI::Settings settings;
		settings.PreferedAPI = AGI::APIType::Guess;
		settings.LoaderFunc = AGI::Window::LoaderFunc;
		settings.MessageFunc = OnAGIMessage;
		settings.Blending = true;

		s_RenderContext = AGI::RenderContext::Create(settings);
		Application::Get().GetWindow() = AGI::Window::Create(Application::Get().GetConfig().WindowProps, s_RenderContext);

		s_RenderContext->Init();

		AGI::TextureSpecification textureSpec;
		textureSpec.Format = AGI::ImageFormat::RGBA;
		textureSpec.Width = 1;
		textureSpec.Height = 1;
		s_WhiteTexture = s_RenderContext->CreateTexture(textureSpec);

		uint32_t whiteTextureData = 0xffffffff;
		s_WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_TextureSlots[0] = s_WhiteTexture;
	}

	void Renderer::InitPipeline(const Ref<RenderPipeline>& pipeline)
	{
		for (const auto& attr : pipeline->Layout)
		{
			auto it = std::find(RenderCaps::RequiredAttrs.begin(), RenderCaps::RequiredAttrs.end(), attr.Name);
			if (it != RenderCaps::RequiredAttrs.end())
				STY_CORE_VERIFY(pipeline->Layout.HasElement(std::string(*it)), "Missing layout element: {}", *it);

			pipeline->AttributeCache[attr.Name] = attr;
		}

		pipeline->VertexArray = s_RenderContext->CreateVertexArray();
		pipeline->VertexBuffer = s_RenderContext->CreateVertexBuffer(RenderCaps::MaxVertices, pipeline->Layout);
		pipeline->VertexArray->AddVertexBuffer(pipeline->VertexBuffer);

		pipeline->VBBase = malloc(pipeline->VertexBuffer->GetSize());

		std::vector<uint32_t> quadIndices(RenderCaps::MaxIndices);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < RenderCaps::MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		pipeline->IndexBuffer = s_RenderContext->CreateIndexBuffer(quadIndices.data(), RenderCaps::MaxIndices);
		pipeline->VertexArray->SetIndexBuffer(pipeline->IndexBuffer);

		if (!pipeline->TextureSampler.empty())
		{
			int32_t samplers[RenderCaps::MaxTextureSlots];
			for (uint32_t i = 0; i < RenderCaps::MaxTextureSlots; i++)
				samplers[i] = i;

			pipeline->Shader->Bind();
			pipeline->Shader->SetIntArray(pipeline->TextureSampler, samplers, RenderCaps::MaxTextureSlots);
		}
	}

	void Renderer::Shutdown()
	{
		s_RenderContext->Shutdown();
	}
	
	void Renderer::BeginRoom(Camera& camera)
	{
		for (auto& [type, pipeline] : s_RenderPipelines)
		{
			pipeline->Shader->Bind();
			pipeline->Shader->SetMat4(pipeline->ProjectionUniform, camera.GetViewProjectionMatrix());

			pipeline->IndexCount = 0;
			pipeline->VBPtr = pipeline->VBBase;
		}

		s_TextureSlotIndex = 1;
	}

	void Renderer::EndRoom()
	{
		for (auto& [type, pipeline] : s_RenderPipelines)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)pipeline->VBPtr - (uint8_t*)pipeline->VBBase);
			pipeline->VertexBuffer->SetData(pipeline->VBBase, dataSize);
		}

		Flush();
	}

	void Renderer::Flush()
	{
		for (uint32_t i = 0; i < s_TextureSlotIndex; ++i)
			s_TextureSlots[i]->Bind(i);

		for (auto& [type, pipeline] : s_RenderPipelines)
		{
			if (pipeline->IndexCount == 0) continue;
			pipeline->Shader->Bind();
			s_RenderContext->DrawIndexed(pipeline->VertexArray, pipeline->IndexCount);
		}
	}

	void Renderer::FlushAndReset()
	{
		EndRoom();

		for (auto& [type, pipeline] : s_RenderPipelines)
		{
			pipeline->IndexCount = 0;
			pipeline->VBPtr = pipeline->VBBase;
		}

		s_TextureSlotIndex = 1;
	}

	float Renderer::GetTextureSlot(const AGI::Texture& texture)
	{
		if (!texture)
			return 0.0f;

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_TextureSlotIndex; i++)
		{
			if (*s_TextureSlots[i] == texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_TextureSlotIndex >= RenderCaps::MaxTextureSlots)
				FlushAndReset();

			textureIndex = (float)(s_TextureSlotIndex);
			s_TextureSlots[s_TextureSlotIndex++] = texture;
		}

		return textureIndex;
	}

}