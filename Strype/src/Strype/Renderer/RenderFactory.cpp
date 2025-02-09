#include "stypch.h"
#include "Strype/Renderer/Renderer.h"

#include "Strype/Renderer/Buffer.h"
#include "Strype/Renderer/RenderAPI.h"
#include "Strype/Renderer/Shader.h"
#include "Strype/Renderer/Texture.h"
#include "Strype/Renderer/VertexArray.h"
#include "Strype/Renderer/Framebuffer.h"

#include "API/OpenGL/OpenGLBuffer.h"
#include "API/OpenGL/OpenGLRenderAPI.h"
#include "API/OpenGL/OpenGLShader.h"
#include "API/OpenGL/OpenGLTexture.h"
#include "API/OpenGL/OpenGLVertexArray.h"
#include "API/OpenGL/OpenGLFramebuffer.h"

namespace Strype {

	RenderAPI::API RenderAPI::s_API = RenderAPI::API::OpenGL;

	Scope<RenderAPI> RenderAPI::Create()
	{
		switch (s_API)
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateScope<OpenGLRenderAPI>();
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(size);
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices, size);
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLShader>(filepath);
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture> Texture::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLTexture>(width, height);
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture> Texture::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLTexture>(path);
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Framebuffer> Framebuffer::Create(int width, int height)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    STY_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(width, height);
		}

		STY_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}