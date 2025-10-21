# Strype [![CMake build](https://github.com/JackJackStudios/Strype/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/JackJackStudios/Strype/actions/workflows/cmake-multi-platform.yml)
**Strype** is a modern, early-stage 2D game engine built for **pixel art**, **super-fast rendering**, and **rapid iteration**.  
Currently tested on **Windows x64** and **Ubuntu ARM64**.

## Features
- Super quick rendering optimized for pixel art
- Robust agnostic renderer (OpenGL - Vulkan comming soon!)
- Intergrates with C# (with .NET / MSBuild)
- Multi-threaded windowing with ImGui

## Using Strype
* Core & Runtime - **Windows or Linux**
* Editor - **Windows only** (DotNet/MSbuild)

To build the entire repository:
```bash
git clone https://github.com/JackJackStudios/Strype
cd Strype
mkdir build && cd build
cmake ..
cmake --build . -j4
```
To create a simple app in Strype:
```cpp
#include <Strype.hpp>

using namespace Strype;

class TestSession : public Session
{
public:
    TestSession()
    {
        WindowProps.Title = "The Strype Engine";
        // Add other window props...
    }
};

int main(int argc, char** argv)
{
    Application(argc, argv)
        .NewSession<TestSession>()
        .Run();
}
```

## Roadmap
Strype’s goal is to create a developer and artist-friendly engine that integrates smoothly with professional tools, making game creation simpler compared to commercial engines.

### Main features to come:
- 2D rendering expansion (text, video, particles)
- More rendering API support (DirectX, Vulkan)
- Integration with tools like [Aseprite](https://www.aseprite.org/), [FMOD](https://www.fmod.com/), and [Steamworks](https://partner.steamgames.com/)
- Flexbox-style UI system
- Templates and packages
- Hot reloading
- Animation system
- Proper audio support
- Export finished games

## Project Structure
- **.github/workflows/** → Config file for automated testing
- **Strype-Editor/** → Windows-only editor
- **Strype-Runtime/** → Cross-platform runtime
- **Strype/** → Core engine code
- **CMakeLists.txt** → Config file for Cmake

## License
Strype is licensed under the [MIT License](LICENSE).
