#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Strype {

	Application* CreateApplication(ApplicationArguments args)
	{
		AppConfig config;
		config.AppName = "Strype-Editor";
		config.WorkingDir = std::string(getenv("STRYPE_DIR")) + "\\Strype-Editor\\";
		config.MasterDir = std::string(getenv("STRYPE_DIR")) + "\\Strype\\master";
		config.DockspaceEnabled = true;
		config.ImGuizmoEnabled = true;

		config.WindowProps.Maximise = true;

		auto* app = new Application(config);
		app->PushPipeline<QuadPipeline>();

		app->PushLayer<EditorLayer>();

		return app;
	}

}