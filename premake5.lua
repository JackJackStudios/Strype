workspace "Strype"
	architecture "x86_64"
	startproject "Strype-Editor"

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Strype"
include "Strype-Editor"

group "Dependencies"
	include "Libraries/GLFW"
	include "Libraries/imgui"
	include "Libraries/yaml-cpp"
group ""