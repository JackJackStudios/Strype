#pragma once

#include "Strype/Core/Base.h"

namespace Strype {

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(int x, int y) = 0;

		virtual int GetWidth() = 0;
		virtual int GetHeight() = 0;

		virtual uint32_t GetAttachmentID() = 0;
		
		static Ref<Framebuffer> Create(int width, int height);
	};

}