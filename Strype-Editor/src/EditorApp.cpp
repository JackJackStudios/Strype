#include <Strype.h>
#include "EditorLayer.h"

using namespace Strype;

int main(int argc, char** argv)
{
	AppConfig config;
	config.AppName = "Strype-Editor";
	config.WorkingDir = std::string(getenv("STRYPE_DIR")) + "\\Strype-Editor\\";
	config.MasterDir = std::string(getenv("STRYPE_DIR")) + "\\Strype\\master";
	config.DockspaceEnabled = true;
	config.ImGuizmoEnabled = true;

	config.WindowProps.Maximise = true;

	Application(config)
		.PushPipeline<QuadPipeline>()
		.PushLayer<EditorLayer>()
		.Run();

	return 0;
}