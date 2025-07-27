#include <Strype.hpp>
#include "RuntimeLayer.hpp"

using namespace Strype;

int main(int argc, char** argv)
{
	AppConfig config;
	config.MasterDir = std::string(getenv("STRYPE_DIR")) + "\\Strype\\master";
	
	Application(config)
		.PushLayer<RuntimeLayer>(argv[1])
		.Run();

	return 0;
}