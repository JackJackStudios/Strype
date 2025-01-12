#include "stypch.h"
#include "Strype/Renderer/Buffer.h"

#include "Strype/Renderer/Renderer.h"

#include "API/OpenGL/OpenGLBuffer.h"

namespace Strype {

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

}