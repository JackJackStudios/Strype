workspace "Strype"
	configurations { "Debug", "Release", "Dist" }
	startproject "Strype-Editor"
    conformancemode "On"

	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	flags { "MultiProcessorCompile" }

	filter "action:vs*"
		linkoptions { "/ignore:4099", "/ignore:4006" }
		disablewarnings { "4068" }

	filter "language:C++ or language:C"
		architecture "x86_64"

	filter "configurations:Debug"
		optimize "Off"
		symbols "On"
		defines "STY_DEBUG"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"
		defines "STY_RELEASE"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
		defines "STY_DIST"

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

include "Strype"
include "Strype-Editor"
include "Strype-Runtime"
include "Strype-ScriptCore"

group "Dependencies"
	include "Libraries/GLFW"
	include "Libraries/imgui"
	include "Libraries/yaml-cpp"
	include "Libraries/Coral/Coral.Native"
	include "Libraries/Coral/Coral.Managed"
	include "Libraries/box2d"
	include "Libraries/agi"
group ""