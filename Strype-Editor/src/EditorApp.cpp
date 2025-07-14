#include <Strype.hpp>
#include "EditorLayer.hpp"

using namespace Strype;

#include <iostream>

int main(int argc, char** argv)
{
#ifndef STY_WINDOWS
	std::cerr << "Strype-Editor currently only supportes Windows." << std::endl;
	STY_DEBUGBREAK();
#endif

	AppConfig config;
	config.AppName = "Strype-Editor";
	config.MasterDir = std::string(getenv("STRYPE_DIR")) + "\\Strype\\master";
	config.DockspaceEnabled = true;

	config.WindowProps.Maximise = true;

	Application(config)
		.PushLayer<EditorLayer>(argc > 1 ? argv[1] : "")
		.Run();

	return 0;
}