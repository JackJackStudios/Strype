#include <Strype.hpp>
#include "LauncherSession.hpp"

using namespace Strype;

int main(int argc, char** argv)
{
    Application(argc, argv)
        .NewSession<LauncherSession>()
        .Run();
}