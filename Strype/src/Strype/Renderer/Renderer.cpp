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
		settings.MessageFunc = OnAGIMessage;
		settings.Blending = true;
		settings.Window = Application::Get().GetConfig().WindowProps;

		Application::Get().GetWindow() = AGI::Window::Create(settings);
		s_RenderContext = AGI::RenderContext::Create(Application::Get().GetWindow());

		s_RenderContext->Init();

		AGI::TextureSpecification textureSpec;
		textureSpec.Format = AGI::ImageFormat::RGBA;
		textureSpec.Width = 1;
		textureSpec.Height = 1;
		s_WhiteTexture = s_RenderContext->CreateTexture(textureSpec);

		uint32_t whiteTextureData = 0xffffffff;
		s_WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_TextureSlots[0] = s_WhiteTexture;

		s_QuadPipeline.Layout = {
			{ AGI::ShaderDataType::Float4, "a_Position" },
			{ AGI::ShaderDataType::Float4, "a_Colour" },
			{ AGI::ShaderDataType::Float2, "a_TexCoord" },
			{ AGI::ShaderDataType::Float,  "a_TexIndex" },
		};
		s_QuadPipeline.TextureSampler = "u_Textures";
		s_QuadPipeline.ProjectionUniform = "u_ViewProjection";
		s_QuadPipeline.ShaderPath = Application::Get().GetConfig().MasterDir / "shaders" / "QuadShader.glsl";

		InitPipeline(s_QuadPipeline);
	}

	void Renderer::Shutdown()
	{
		s_RenderContext->Shutdown();
	}

	void Renderer::InitPipeline(RenderPipeline& pipeline)
	{
		for (const auto& attr : pipeline.Layout)
		{
			auto it = std::find(RenderCaps::RequiredAttrs.begin(), RenderCaps::RequiredAttrs.end(), attr.Name);
			if (it != RenderCaps::RequiredAttrs.end())
				STY_CORE_VERIFY(pipeline.Layout.HasElement(std::string(*it)), "Missing layout element: {}", *it);
		}

		pipeline.VertexArray = s_RenderContext->CreateVertexArray();
		pipeline.VertexBuffer = s_RenderContext->CreateVertexBuffer(RenderCaps::MaxVertices, pipeline.Layout);
		pipeline.VertexArray->AddVertexBuffer(pipeline.VertexBuffer);

		pipeline.VBBase = malloc(pipeline.VertexBuffer->GetSize());

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

		pipeline.IndexBuffer = s_RenderContext->CreateIndexBuffer(quadIndices.data(), RenderCaps::MaxIndices);
		pipeline.VertexArray->SetIndexBuffer(pipeline.IndexBuffer);

		pipeline.Shader = s_RenderContext->CreateShader(AGI::ProcessSource(Utils::ReadFile(pipeline.ShaderPath)));

		if (!pipeline.TextureSampler.empty())
		{
			int32_t samplers[RenderCaps::MaxTextureSlots];
			for (uint32_t i = 0; i < RenderCaps::MaxTextureSlots; i++)
				samplers[i] = i;

			pipeline.Shader->SetIntArray(pipeline.TextureSampler, samplers, RenderCaps::MaxTextureSlots);
		}
	}
	
	void Renderer::BeginRoom(Camera& camera)
	{
		s_QuadPipeline.nextAttr = 0;
		s_QuadPipeline.Shader->SetMat4(s_QuadPipeline.ProjectionUniform, camera.GetViewProjectionMatrix());
		
		s_QuadPipeline.IndexCount = 0;
		s_QuadPipeline.VBPtr = s_QuadPipeline.VBBase;

		s_TextureSlotIndex = 1;
	}

	void Renderer::EndRoom()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_QuadPipeline.VBPtr - (uint8_t*)s_QuadPipeline.VBBase);
		s_QuadPipeline.VertexBuffer->SetData(s_QuadPipeline.VBBase, dataSize);

		Flush();
	}

	void Renderer::Flush()
	{
		for (uint32_t i = 0; i < s_TextureSlotIndex; ++i)
			s_TextureSlots[i]->Bind(i);

		if (s_QuadPipeline.IndexCount == 0) return;
		s_QuadPipeline.Shader->Bind();
		s_RenderContext->DrawIndexed(s_QuadPipeline.VertexArray, s_QuadPipeline.IndexCount);
	}

	void Renderer::FlushAndReset()
	{
		EndRoom();

		s_QuadPipeline.IndexCount = 0;
		s_QuadPipeline.VBPtr = s_QuadPipeline.VBBase;

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