#include "stypch.h"
#include "Strype/Renderer/Shader.h"

#include "Strype/Renderer/Renderer.h"
#include "API/OpenGL/OpenGLShader.h"

namespace Strype {

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
}