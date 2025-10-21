#pragma once

#include <AGI/agi.hpp>

#include "Strype/Utils/PlatformUtils.hpp"

namespace Strype {

	using TexCoords = std::array<glm::vec2, 4>;

	namespace Utils {

		static constexpr TexCoords FlipTexCoordsH(const TexCoords& tex)
		{
			return { tex[1], tex[0], tex[3], tex[2] };
		}

		static constexpr TexCoords FlipTexCoordsV(const TexCoords& tex)
		{
			return { tex[3], tex[2], tex[1], tex[0] };
		}

		static constexpr glm::vec2 SizeFromTexcoords(const TexCoords& tex, const glm::vec2& atlas_size)
		{
			return { atlas_size.x * glm::abs(tex[1].x - tex[0].x), atlas_size.y * glm::abs(tex[1].y - tex[2].y) };
		}

		static constexpr glm::vec2 PixelToTexcoords(const glm::vec2& pos, int atlas_width, int atlas_height)
		{
			return { pos.x / atlas_width, pos.y / atlas_height };
		}

		static constexpr TexCoords BoxToTextureCoords(const glm::vec2& pos, int width, int height, const glm::vec2& atlas_size)
		{
			return {
				PixelToTexcoords(pos, atlas_size.x, atlas_size.y),
				PixelToTexcoords({ pos.x + width, pos.y }, atlas_size.x, atlas_size.y),
				PixelToTexcoords({ pos.x + width, pos.y + height }, atlas_size.x, atlas_size.y),
				PixelToTexcoords({ pos.x, pos.y + height }, atlas_size.x, atlas_size.y),
			};
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

		void SubmitAttribute(const std::string& name, const Buffer& buf)
		{
			if (buf.Empty())
				return;

			STY_VERIFY(buf.Size == Layout[NextAttr].Size, "Size of attribute and buffer entered must be the same!");
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