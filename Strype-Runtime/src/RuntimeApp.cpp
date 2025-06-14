#include <Strype.hpp>
#include "RuntimeLayer.hpp"

using namespace Strype;

int main(int argc, char** argv)
{
	AppConfig config;
	config.AppName = "Strype-Runtime";
	config.MasterDir = std::string(getenv("STRYPE_DIR")) + "\\Strype\\master";
	config.ImGuiEnabled = false;

	Application(config)
		.PushPipeline<QuadPipeline>()
		.PushLayer<RuntimeLayer>(argv[1])
		.Run();

	return 0;
}