# Strype

Strype is a early-stage game engine built for pixel art, super quick rendering and fast iteration speed. We aim eventually to have support for Vulkan, Asesprite intergation and multiplayer games

***

## Getting Started
Visual Studio 2022 is recommended, Strype is officially untested on other development environments whilst we focus on a Windows build.

<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with `git clone --recursive https://github.com/JackKnox/Strype`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

<ins>**2. Configuring the dependencies:**</ins>

Run the [Setup.bat](https://github.com/JackKnox/Strype/blob/main/scripts/Setup.bat) file found in `scripts` folder. This will download the required prerequisites for the project if they are not present yet.If changes are made, or if you want to regenerate project files, rerun the [build.bat](https://github.com/JackKnox/Strype/blob/master/scripts/build.bat) script file found in `scripts` folder.

***

### Main features to come:
- Fast 2D rendering (UI, particles, animations, etc.)
- Support for Mac, Linux, Android and iOS
    - Native rendering API support (DirectX, Vulkan, Metal)
- Fully featured viewer and editor applications
- Fully scripted interaction and behavior
- Integrated 3rd party 2D physics engine
- Procedural terrain and world generation
- Artificial Intelligence
