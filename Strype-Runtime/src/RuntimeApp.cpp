#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "RuntimeLayer.h"

namespace Strype {

	class Runtime : public Application
	{
	public:
		Runtime(const AppConfig& config)
			: Application(config)
		{
			PushLayer(new RuntimeLayer());
		}

		~Runtime()
		{

		}

	};

	Application* CreateApplication()
	{
		AppConfig config;
		config.AppName = "Strype-Runtime";
		config.ShaderPath = std::string(getenv("STRYPE_DIR")) + "\\Strype-Editor\\assets\\BaseShader.glsl";
		config.StartupFrames = 10;
		config.ImGuiEnabled = false;
		config.DockspaceEnabled = false;
		config.ImGuizmoEnabled = false;
 
		config.RendererLayout = {
			{ AGI::ShaderDataType::Float3, "a_Position" },
			{ AGI::ShaderDataType::Float4, "a_Colour" },
			{ AGI::ShaderDataType::Float2, "a_TexCoord" },
			{ AGI::ShaderDataType::Float, "a_TexIndex" },
		};

		return new Runtime(config);
	}

}