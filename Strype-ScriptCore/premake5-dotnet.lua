-- Non-Windows split premake project

StrypeDirectory = os.getenv("STRYPE_DIR")
include (path.join(StrypeDirectory, "Libraries", "Coral", "Premake", "CSExtensions.lua"))

workspace "Strype-ScriptCore"
	configurations { "Debug", "Release", "Dist" }

	filter "configurations:Debug"
		optimize "Off"
		symbols "On"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"

	include (path.join(StrypeDirectory, "Libraries", "Coral", "Coral.Managed"))

	project "Strype-ScriptCore"
		kind "SharedLib"
		language "C#"
		dotnetframework "net8.0"
		clr "Unsafe"
		targetdir (path.join(StrypeDirectory, "Strype-Editor", "DotNet"))
		objdir (path.join(StrypeDirectory, "Strype-Editor", "DotNet", "Intermediates"))

		--linkAppReferences(false)

		links { "Coral.Managed" }

		propertytags {
			{ "AppendTargetFrameworkToOutputPath", "false" },
			{ "Nullable", "enable" },
		}

		files {
			"Source/**.cs",
		}
