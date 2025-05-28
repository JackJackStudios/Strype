#pragma once

#include <AGI/agi.hpp>
#include <any>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Strype {

	class RenderPipeline
	{
	public:
		uint32_t IndexCount = 0;
		void* VBBase = nullptr;
		void* VBPtr = nullptr;

		std::string TextureSampler;

		std::shared_ptr<AGI::VertexArray> VertexArray;
		std::shared_ptr<AGI::VertexBuffer> VertexBuffer;
		std::shared_ptr<AGI::IndexBuffer> IndexBuffer;
		std::shared_ptr<AGI::Shader> Shader;
		AGI::BufferLayout Layout;

		std::unordered_map<std::string, AGI::BufferElement> AttributeCache;
		std::unordered_map<std::string, std::any> UserAttributes;

		virtual void DrawPrimitive(const glm::mat4& transform, const glm::vec4& colour, const glm::vec2 texcoords[], float textureIndex) = 0;

		void SubmitAttribute(const std::string& name, const std::any& value)
		{
			UserAttributes[name] = value;
		}
	};

	class QuadPipeline : public RenderPipeline
	{
	public:
		QuadPipeline();

		virtual void DrawPrimitive(const glm::mat4& transform, const glm::vec4& colour, const glm::vec2 texcoords[], float textureIndex) override;
	};

	class TextPipeline : public RenderPipeline
	{
	public:
		TextPipeline();
		~TextPipeline();

		virtual void DrawPrimitive(const glm::mat4& transform, const glm::vec4& colour, const glm::vec2 texcoords[], float textureIndex) override;
	private:
		FT_Library m_FreetypeLib;

		friend class FontSerializer;
	};

}