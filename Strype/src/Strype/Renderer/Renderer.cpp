#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

#include "Strype/Core/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <numeric>
#include <GLFW/glfw3.h>

namespace Strype {

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
		s_RenderAPI = AGI::RenderAPI::Create(
		{
			.PreferedAPI = AGI::APIType::Guess,
			.Blending = true,

			.LoaderFunc = (AGI::OpenGLloaderFn)glfwGetProcAddress,
			.MessageFunc = OnAGIMessage,
		});

		RenderPipelines.Set<QuadPipeline>(CreateRef<QuadPipeline>());

		for (auto& [type, pipeline] : RenderPipelines)
		{
			for (const auto& attr : pipeline->Layout)
			{
				auto it = std::find(RenderCaps::RequiredAttrs.begin(), RenderCaps::RequiredAttrs.end(), attr.Name);
				if (it != RenderCaps::RequiredAttrs.end())
					STY_CORE_VERIFY(pipeline->Layout.HasElement(std::string(*it)), "Missing layout element: {}", *it);

				pipeline->AttributeCache[attr.Name] = attr;
			}

			pipeline->VertexArray = AGI::VertexArray::Create();
			pipeline->VertexBuffer = AGI::VertexBuffer::Create(RenderCaps::MaxVertices, pipeline->Layout);
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

			pipeline->IndexBuffer = AGI::IndexBuffer::Create(quadIndices.data(), RenderCaps::MaxIndices);
			pipeline->VertexArray->SetIndexBuffer(pipeline->IndexBuffer);
		}

		AGI::TextureSpecification textureSpec;
		textureSpec.Format = AGI::ImageFormat::RGBA;
		textureSpec.Width = 1;
		textureSpec.Height = 1;
		WhiteTexture = AGI::Texture::Create(textureSpec);

		uint32_t whiteTextureData = 0xffffffff;
		WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		TextureSlots[0] = WhiteTexture;

		/*
		int sucess = FT_Init_FreeType(&s_Data.FreetypeLib);
		STY_CORE_VERIFY(sucess == 0, "Could not init FreeType");

		FT_New_Face(s_Data.FreetypeLib, "assets/test-font.ttf", 0, &s_Data.t0);
		FT_Set_Pixel_Sizes(s_Data.t0, 0, 48);

		for (uint8_t i = 0; i < 128; i++)
		{
			if (FT_Load_Char(s_Data.t0, i, FT_LOAD_RENDER) != 0)
				continue;

			FT_Bitmap* bitmap = &s_Data.t0->glyph->bitmap;
			unsigned char* flippedBuffer = (unsigned char*)malloc(bitmap->width * bitmap->rows * sizeof(unsigned char));

			for (unsigned int y = 0; y < bitmap->rows; ++y) {
				memcpy(
					flippedBuffer + (bitmap->rows - 1 - y) * bitmap->width,
					bitmap->buffer + y * bitmap->width,
					bitmap->width
				);
			}

			AGI::TextureSpecification textureSpec;
			textureSpec.Width = bitmap->width;
			textureSpec.Height = bitmap->rows;
			textureSpec.Format = AGI::ImageFormat::RED;

			std::shared_ptr<AGI::Texture> texture = AGI::Texture::Create(textureSpec);
			texture->SetData(flippedBuffer, bitmap->width * bitmap->rows * sizeof(unsigned char));

			Character character;
			character.Texture = texture;
			character.Size = glm::ivec2(bitmap->width, bitmap->rows);
			character.Bearing = glm::ivec2(s_Data.t0->glyph->bitmap_left, s_Data.t0->glyph->bitmap_top);
			character.Advance = s_Data.t0->glyph->advance.x;

			s_Data.t1[i] = character;
			free(flippedBuffer);
		}
		*/
	}

	void Renderer::Shutdown()
	{
	}
	
	void Renderer::BeginRoom(Camera& camera)
	{
		for (auto& [type, pipeline] : RenderPipelines)
		{
			pipeline->Shader->Bind();
			pipeline->Shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

			pipeline->IndexCount = 0;
			pipeline->VBPtr = pipeline->VBBase;
		}

		TextureSlotIndex = 1;
	}

	void Renderer::EndRoom()
	{
		for (auto& [type, pipeline] : RenderPipelines)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)pipeline->VBPtr - (uint8_t*)pipeline->VBBase);
			pipeline->VertexBuffer->SetData(pipeline->VBBase, dataSize);
		}

		Flush();
	}

	void Renderer::Flush()
	{
		for (uint32_t i = 0; i < TextureSlotIndex; ++i)
			TextureSlots[i]->Bind(i);

		for (auto& [type, pipeline] : RenderPipelines)
		{
			if (pipeline->IndexCount == 0) continue;
			s_RenderAPI->DrawIndexed(pipeline->VertexArray, pipeline->IndexCount);
		}
	}

	void Renderer::FlushAndReset()
	{
		EndRoom();

		for (auto& [type, pipeline] : RenderPipelines)
		{
			pipeline->IndexCount = 0;
			pipeline->VBPtr = pipeline->VBBase;
		}

		TextureSlotIndex = 1;
	}

	float Renderer::GetTextureSlot(const Ref<AGI::Texture>& texture)
	{
		if (!texture)
			return 0.0f;

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < TextureSlotIndex; i++)
		{
			if (*TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (TextureSlotIndex >= RenderCaps::MaxTextureSlots)
				FlushAndReset();

			textureIndex = (float)(TextureSlotIndex);
			TextureSlots[TextureSlotIndex++] = texture;
		}

		return textureIndex;
	}

}