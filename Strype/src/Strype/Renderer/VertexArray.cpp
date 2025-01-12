#include "stypch.h"
#include "Strype/Renderer/VertexArray.h"

#include "Strype/Renderer/Renderer.h"
#include "API/OpenGL/OpenGLVertexArray.h"

namespace Strype {

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

}