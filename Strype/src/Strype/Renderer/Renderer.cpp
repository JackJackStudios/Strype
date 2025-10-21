#include "stypch.hpp"
#include "Renderer.hpp"

#include "Strype/Core/Application.hpp"
#include "../baseshaders.embed"

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
		textureSpec.Size = { 1, 1 };
		textureSpec.Format = AGI::ImageFormat::RGBA;
		m_WhiteTexture = m_RenderContext->CreateTexture(textureSpec);

		uint32_t whiteTextureData = 0xffffffff;
		m_WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		m_TextureSlots[0] = m_WhiteTexture;

		InitPipeline(m_QuadPipeline, QuadShader_data);
		FT_Init_FreeType(&m_FreetypeLib);
	}

	Renderer::~Renderer()
	{
		FT_Done_FreeType(m_FreetypeLib);

		m_RenderContext->Shutdown();
		delete m_RenderContext;
	}

	void Renderer::InitPipeline(RenderPipeline& pipeline, const char* shaderSource)
	{
		pipeline.Shader = m_RenderContext->CreateShader(AGI::Utils::ProcessSource(shaderSource));
		pipeline.Layout = pipeline.Shader->GetLayout();

		for (const auto& attr : pipeline.Layout)
		{
			auto it = std::find(RenderCaps::RequiredAttrs.begin(), RenderCaps::RequiredAttrs.end(), attr.Name);
			if (it != RenderCaps::RequiredAttrs.end())
				STY_VERIFY(pipeline.Layout.HasElement(std::string(*it)), "Missing layout element: {}", *it);
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

		pipeline.IndexBuffer = m_RenderContext->CreateIndexBuffer(quadIndices.data(), quadIndices.size());
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
		m_QuadPipeline.VertexBuffer->SetData(m_QuadPipeline.VBBase, (uint32_t)((uint8_t*)m_QuadPipeline.VBPtr - (uint8_t*)m_QuadPipeline.VBBase));

		Flush();
	}

	void Renderer::Flush()
	{
		for (uint32_t i = 0; i < m_TextureSlotIndex; ++i)
			m_TextureSlots[i]->Bind(i);

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

	float Renderer::GetTextureSlot(AGI::Texture texture)
	{
		if (!texture) return 0.0f;
		for (uint32_t i = 1; i < m_TextureSlotIndex; i++)
		{
			if (m_TextureSlots[i]->GetRendererID() == texture->GetRendererID())
				return (float)i;
		}

		if (m_TextureSlotIndex >= RenderCaps::MaxTextureSlots)
			FlushAndReset();

		uint32_t slotIndex = m_TextureSlotIndex++;
		m_TextureSlots[slotIndex] = texture;

		return (float)slotIndex;
	}

}