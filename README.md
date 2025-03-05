# Strype

Strype is a early-stage game engine built for pixel art, super quick rendering and fast iteration speed.

***

## Libraries
1. GLFW
2. ImGui
3. yaml-cpp
4. entt
5. glm
6. glad

***

## Getting Started
Visual Studio 2022 is recommended, Strype is officially untested on other development environments whilst we focus on a Windows build.

<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with `git clone --recursive https://github.com/JackKnox/Strype`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

<ins>**2. Configuring the dependencies:**</ins>

Run the [Setup.bat](https://github.com/JackKnox/Strype/blob/main/scripts/Setup.bat) file found in `scripts` folder. This will download the required prerequisites for the project if they are not present yet.If changes are made, or if you want to regenerate project files, rerun the [build.bat](https://github.com/JackKnox/Strype/blob/master/scripts/build.bat) script file found in `scripts` folder.