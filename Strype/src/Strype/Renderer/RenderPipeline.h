#pragma once

#include <AGI/agi.hpp>
#include <any>

namespace Strype {

	using TexCoords = std::array<glm::vec2, 4>;

	class RenderPipeline
	{
	public:
		uint32_t IndexCount = 0;
		void* VBBase = nullptr;
		void* VBPtr = nullptr;

		std::string TextureSampler;
		std::string ProjectionUniform;

		std::shared_ptr<AGI::VertexArray> VertexArray;
		std::shared_ptr<AGI::VertexBuffer> VertexBuffer;
		std::shared_ptr<AGI::IndexBuffer> IndexBuffer;
		std::shared_ptr<AGI::Shader> Shader;
		AGI::BufferLayout Layout;

		std::unordered_map<std::string, AGI::BufferElement> AttributeCache;
		std::unordered_map<std::string, std::any> UserAttributes;

		virtual void DrawPrimitive(const glm::mat4& transform, const glm::vec4& colour, const TexCoords& texcoords, float textureIndex) = 0;

		void SubmitAttribute(const std::string& name, const std::any& value)
		{
			UserAttributes[name] = value;
		}
	};

	class QuadPipeline : public RenderPipeline
	{
	public:
		QuadPipeline();

		virtual void DrawPrimitive(const glm::mat4& transform, const glm::vec4& colour, const TexCoords& texcoords, float textureIndex) override;
	};

}