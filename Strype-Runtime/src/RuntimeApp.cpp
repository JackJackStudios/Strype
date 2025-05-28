#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "RuntimeLayer.h"

namespace Strype {

	Application* CreateApplication(ApplicationArguments args)
	{
		AppConfig config;
		config.AppName = "Strype-Runtime";
		config.MasterDir = std::string(getenv("STRYPE_DIR")) + "\\Strype\\master";
		config.ImGuiEnabled = false;

		auto* app = new Application(config);
		app->PushLayer<RuntimeLayer>(args[1]);

		return app;
	}

}