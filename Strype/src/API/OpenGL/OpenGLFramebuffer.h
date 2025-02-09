#pragma once

#include "Strype/Renderer/Framebuffer.h"

namespace Strype {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(int width, int height);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(int x, int y) override;

		virtual int GetWidth() override { return m_Width; };
		virtual int GetHeight() override { return m_Height; };

		virtual uint32_t GetAttachmentID() override { return m_ColourAttachment; };

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColourAttachment = 0;

		uint32_t m_Width;
		uint32_t m_Height;
	};

}