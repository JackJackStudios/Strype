#include <Strype.hpp>
#include "RuntimeSession.hpp"

using namespace Strype;

int main(int argc, char** argv)
{
	Application(argc, argv)
		.NewSession<RuntimeSession>(argc > 1 ? argv[1] : FileDialogs::OpenFile("Strype Project (.sproj)\0*.sproj\0"))
		.Run();

	return 0;
}