#include "stypch.h"
#include "API/OpenGL/OpenGLRenderAPI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Strype {

	void OpenGLRenderAPI::Init(void* window)
	{
		glfwMakeContextCurrent((GLFWwindow*)window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		STY_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLRenderAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRenderAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

}