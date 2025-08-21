#include <Strype.hpp>
#include "RuntimeSession.hpp"

using namespace Strype;

int main(int argc, char** argv)
{
	AppConfig config;
	config.MasterDir = std::string(getenv("STRYPE_DIR")) + "\\Strype\\master";
	
	Application(config)
		.NewSession<RuntimeSession>(argv[1])
		.Run();

	return 0;
}