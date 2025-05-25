StrypeDirectory = os.getenv("STRYPE_DIR")

workspace "Strype-ScriptCore"
	architecture "universal"
	startproject "Strype-ScriptCore"

	configurations {
		"Debug",
		"Release",
	}

include "%{StrypeDirectory}/Strype-ScriptCore/coral-src/Premake/CSExtensions.lua"
include "%{StrypeDirectory}/Strype-ScriptCore/coral-src/Coral.Managed"

project "Strype-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "net8.0"
	clr "Unsafe"
	targetdir "%{StrypeDirectory}/Strype/master/DotNet"
	objdir "%{StrypeDirectory}/Strype/master/DotNet/Intermediates"

	links {
		"Coral.Managed"
	}

	files {
		"%{StrypeDirectory}/Strype-ScriptCore/Source/**.cs",
	}