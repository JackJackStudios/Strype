#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Strype {

	Application* CreateApplication(ApplicationArguments args)
	{
		AppConfig config;
		config.AppName = "Strype-Editor";
		config.WorkingDir = std::string(getenv("STRYPE_DIR")) + "\\Strype-Editor\\";
		config.DockspaceEnabled = true;
		config.ImGuizmoEnabled = true;

		config.WindowProps.Maximise = true;
 
		config.RendererLayout = {
			{ AGI::ShaderDataType::Float3, "a_Position" },
			{ AGI::ShaderDataType::Float4, "a_Colour" },
			{ AGI::ShaderDataType::Float2, "a_TexCoord" },
			{ AGI::ShaderDataType::Float,  "a_TexIndex" },
			{ AGI::ShaderDataType::Int,    "a_ObjectID" },
		};

		auto* app = new Application(config);
		app->PushLayer<EditorLayer>();

		return app;
	}

}