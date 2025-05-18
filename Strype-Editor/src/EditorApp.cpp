#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Strype {

	class Editor : public Application
	{
	public:
		Editor(const AppConfig& config)
			: Application(config)
		{
			PushLayer(new EditorLayer());
		}

		~Editor()
		{

		}

	};

	Application* CreateApplication()
	{
		AppConfig config;
		config.AppName = "Strype-Editor";
		config.ShaderPath = "assets\\BaseShader.glsl";
		config.WorkingDir = std::string(getenv("STRYPE_DIR")) + "\\Strype-Editor\\";
		config.StartupFrames = 10;
		config.ImGuiEnabled = true;
		config.DockspaceEnabled = true;
		config.ImGuizmoEnabled = true;
 
		config.RendererLayout = {
			{ AGI::ShaderDataType::Float3, "a_Position" },
			{ AGI::ShaderDataType::Float4, "a_Colour" },
			{ AGI::ShaderDataType::Float2, "a_TexCoord" },
			{ AGI::ShaderDataType::Float,  "a_TexIndex" },
			{ AGI::ShaderDataType::Int,    "a_ObjectID" },
		};

		return new Editor(config);
	}

}