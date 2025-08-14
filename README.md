# Strype
[![CMake build](https://github.com/JackJackStudios/Strype/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/JackJackStudios/Strype/actions/workflows/cmake-multi-platform.yml)

## Introduction
Strype is a early-stage game engine built for pixel art, super quick rendering and fast iteration speed.
The engine is officially tested on Windows x64 and Unbuntu ARM64.

## Requirements 
* Windows or Linux (x64 or ARM64)
* CMake 3.15
* A C++ 20 compiler 

## Building Strype
Core & Runtime - **Windows or Linux**

Editor - **Windows only** (DotNet/MSbuild)

To build the entire repository:
```console
git clone https://github.com/JackJackStudios/Strype
cd Strype
mkdir build && cd build
cmake ..
cmake --build .
```

## License
AGI is licensed under the [MIT License](LICENSE).