#pragma once

#include <AGI/agi.hpp>

#include "Strype/Utils/PlatformUtils.hpp"

namespace Strype {

	using TexCoords = std::array<glm::vec2, 4>;

	namespace Utils {

		static constexpr TexCoords FlipTexCoords(const TexCoords& tex)
		{
			return { tex[3], tex[2], tex[1], tex[0] };
		}

	};

	struct RenderPipeline
	{
		uint32_t IndexCount = 0;
		void* VBBase = nullptr;
		void* VBPtr = nullptr;

		AGI::VertexArray VertexArray;
		AGI::VertexBuffer VertexBuffer;
		AGI::IndexBuffer IndexBuffer;
		AGI::Shader Shader;
		AGI::BufferLayout Layout;

		int NextAttr = 0;
		std::string UserAttribute;

		void SubmitAttribute(const std::string& name, const Buffer& buf)
		{
			if (buf.Empty())
				return;

			STY_CORE_VERIFY(buf.Size == Layout[NextAttr].Size, "Size of attribute and buffer entered must be the same!");
			std::memcpy(Utils::ShiftPtr(VBPtr, Layout[NextAttr].Offset), buf.Data, Layout[NextAttr].Size);

			NextAttr++;
		}

		void NextPoint()
		{
			VBPtr = Utils::ShiftPtr(VBPtr, VertexBuffer->GetLayout().GetStride());
			NextAttr = 0;
		}

		void NextFrame()
		{
			IndexCount = 0;
			VBPtr = VBBase;
			NextAttr = 0;
		}

		template<typename T>
		void SubmitAttribute(const std::string& name, const T value) { SubmitAttribute(name, Buffer(value)); }
	};

}