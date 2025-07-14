#pragma once

#include <AGI/agi.hpp>

#include "Strype/Utils/PlatformUtils.hpp"

namespace Strype {

	using TexCoords = std::array<glm::vec2, 4>;

	struct RenderPipeline
	{
		uint32_t IndexCount = 0;
		void* VBBase = nullptr;
		void* VBPtr = nullptr;

		std::string TextureSampler;
		std::string ProjectionUniform;
		std::string UserAttribute;
		std::filesystem::path ShaderPath;

		AGI::VertexArray VertexArray;
		AGI::VertexBuffer VertexBuffer;
		AGI::IndexBuffer IndexBuffer;
		AGI::Shader Shader;
		AGI::BufferLayout Layout;

		int nextAttr = 0;

		template<typename T>
		void SubmitAttribute(const std::string& name, const T value)
		{
			STY_CORE_VERIFY(sizeof(T) == Layout[nextAttr].Size, "Attribute and value entered must be the same!");
			std::memcpy(Utils::ShiftPtr(VBPtr, Layout[nextAttr].Offset), &value, Layout[nextAttr].Size);

			nextAttr++;
		}

		void SubmitAttribute(const std::string& name, const Buffer& buf)
		{
			if (buf.Empty())
				return;

			STY_CORE_VERIFY(buf.Size == Layout[nextAttr].Size, "Attribute and buffer entered must be the same!");
			std::memcpy(Utils::ShiftPtr(VBPtr, Layout[nextAttr].Offset), buf.Data, Layout[nextAttr].Size);

			nextAttr++;
		}

		void NextPoint()
		{
			VBPtr = Utils::ShiftPtr(VBPtr, VertexBuffer->GetLayout().GetStride());
			nextAttr = 0;
		}
	};

}