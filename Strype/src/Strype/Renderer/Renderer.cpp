 #include "stypch.hpp"
#include "Renderer.hpp"

#include "Strype/Core/Application.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Strype {

	Renderer::Renderer(AGI::Window* window)
	{ 
		m_RenderContext = AGI::RenderContext::Create(window);
	}

	void Renderer::Init()
	{
		m_RenderContext->Init();
		m_CurrentContext = this;

		AGI::TextureSpecification textureSpec;
		textureSpec.Format = AGI::ImageFormat::RGBA;
		textureSpec.Width = 1;
		textureSpec.Height = 1;
		m_WhiteTexture = m_RenderContext->CreateTexture(textureSpec);

		uint32_t whiteTextureData = 0xffffffff;
		m_WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		m_TextureSlots[0] = { nullptr, m_WhiteTexture };

		InitPipeline(m_QuadPipeline, "QuadShader.glsl");
	}

	void Renderer::Shutdown()
	{
		m_RenderContext->Shutdown();
		delete m_RenderContext;
	}

	void Renderer::InitPipeline(RenderPipeline& pipeline, const std::filesystem::path& filepath)
	{
		pipeline.Shader = m_RenderContext->CreateShader(AGI::Utils::ProcessSource(Utils::ReadFile(Application::Get().GetConfig().MasterDir / "shaders" / filepath)));
		pipeline.Layout = pipeline.Shader->GetLayout();

		for (const auto& attr : pipeline.Layout)
		{
			auto it = std::find(RenderCaps::RequiredAttrs.begin(), RenderCaps::RequiredAttrs.end(), attr.Name);
			if (it != RenderCaps::RequiredAttrs.end())
				STY_CORE_VERIFY(pipeline.Layout.HasElement(std::string(*it)), "Missing layout element: {}", *it);
		}

		if (pipeline.Layout.GetSize() > RenderCaps::RequiredAttrs.size())
		{
			std::string userAttr = pipeline.Layout[RenderCaps::RequiredAttrs.size()].Name;

			STY_CORE_TRACE("Detected user attribute in {}: \"{}\" ", filepath.filename(), userAttr);
			pipeline.UserAttribute = userAttr;
		}

		pipeline.VertexArray = m_RenderContext->CreateVertexArray();
		pipeline.VertexBuffer = m_RenderContext->CreateVertexBuffer(RenderCaps::MaxVertices, pipeline.Layout);
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

		pipeline.IndexBuffer = m_RenderContext->CreateIndexBuffer(quadIndices.data(), RenderCaps::MaxIndices);
		pipeline.VertexArray->SetIndexBuffer(pipeline.IndexBuffer);

		int32_t samplers[RenderCaps::MaxTextureSlots];
		for (uint32_t i = 0; i < RenderCaps::MaxTextureSlots; i++)
			samplers[i] = i;

		pipeline.Shader->SetIntArray("u_Textures", samplers, RenderCaps::MaxTextureSlots);
	}
	
	void Renderer::BeginRoom(Camera& camera)
	{
		m_QuadPipeline.NextFrame();
		m_QuadPipeline.Shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		
		m_TextureSlotIndex = 1;
	}

	void Renderer::EndRoom()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadPipeline.VBPtr - (uint8_t*)m_QuadPipeline.VBBase);
		m_QuadPipeline.VertexBuffer->SetData(m_QuadPipeline.VBBase, dataSize);

		Flush();
	}

	void Renderer::Flush()
	{
		for (uint32_t i = 0; i < m_TextureSlotIndex; ++i)
			m_TextureSlots[i].Texture->Bind(i);

		if (m_QuadPipeline.IndexCount == 0) return;
		m_QuadPipeline.Shader->Bind();
		m_RenderContext->DrawIndexed(m_QuadPipeline.VertexArray, m_QuadPipeline.IndexCount);
	}

	void Renderer::FlushAndReset()
	{
		EndRoom();

		m_QuadPipeline.IndexCount = 0;
		m_QuadPipeline.VBPtr = m_QuadPipeline.VBBase;

		m_TextureSlotIndex = 1;
	}

	float Renderer::GetTextureSlot(Ref<Sprite> sprite)
	{
		if (!sprite) return 0.0f;

		float slotIndex = 0.0f;
		for (uint32_t i = 1; i < m_TextureSlotIndex; i++)
		{
			if (m_TextureSlots[i].SpriteRef == sprite)
			{
				slotIndex = (float)i;
				break;
			}
		}

		if (slotIndex == 0.0f)
		{
			if (m_TextureSlotIndex >= RenderCaps::MaxTextureSlots)
				FlushAndReset();
			
			slotIndex = (float)m_TextureSlotIndex;

			TextureSlot slot;
			slot.SpriteRef = sprite;
			slot.Texture = m_RenderContext->CreateTexture(sprite->GetSpecs());
			m_TextureSlots[m_TextureSlotIndex++] = slot;
		}

		return slotIndex;
	}

	AGI::Texture Renderer::GetTexture(Ref<Sprite> sprite)
	{
		return m_TextureSlots[GetTextureSlot(sprite)].Texture;
	}

}