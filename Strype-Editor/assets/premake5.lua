FileVersion = 1.2

StrypeDirectory = os.getenv("STRYPE_DIR")
include (path.join(StrypeDirectory, "Libraries", "Coral", "Premake", "CSExtensions.lua"))

workspace "{0}"
	startproject "{0}"
	configurations { "Debug", "Release", "Dist" }

	project "{0}"
		kind "SharedLib"
		language "C#"
		dotnetframework "net8.0"

		targetname "{0}"
		targetdir ("%{prj.location}/strype/Binaries")
		objdir ("%{prj.location}/strype/Intermediates")

		propertytags {
			{ "AppendTargetFrameworkToOutputPath", "false" },
			{ "Nullable", "enable" },
		}

		files  {
			"**.cs", 
		}

		links {
			"Strype-ScriptCore",
			"Coral.Managed",
		}	

		filter "configurations:Debug"
			optimize "Off"
			symbols "Default"

		filter "configurations:Release"
			optimize "On"
			symbols "Default"

		filter "configurations:Dist"
			optimize "Full"
			symbols "Off"

	group "Strype"
		include (path.join(StrypeDirectory, "Libraries", "Coral", "Coral.Managed"))
		include (path.join(StrypeDirectory, "Strype-ScriptCore"))
	group ""