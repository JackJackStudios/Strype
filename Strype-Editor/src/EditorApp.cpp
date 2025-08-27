#include <iostream>

#include <Strype.hpp>
#include "LauncherSession.hpp"

using namespace Strype;

int main(int argc, char** argv)
{
#ifndef STY_WINDOWS
	// NOTE: Using std::cerr as spdlog is not initialized yet.
	std::cerr << "Strype-Editor currently only supportes Windows." << std::endl;
	STY_DEBUGBREAK();
#endif

	AppConfig config;
	config.MasterDir = std::string(getenv("STRYPE_DIR")) + "\\Strype\\master";

	Application(config)
		.NewSession<LauncherSession>()
		.Run();

	return 0;
}