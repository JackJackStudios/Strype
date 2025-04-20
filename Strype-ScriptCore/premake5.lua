StrypeDirectory = os.getenv("STRYPE_DIR")

include (path.join(StrypeDirectory, "Libraries", "Coral", "Premake", "CSExtensions.lua"))
include (path.join(StrypeDirectory, "Libraries", "Coral", "Coral.Managed"))

project "Strype-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "net8.0"
	clr "Unsafe"
	targetdir "%{StrypeDirectory}/Strype-Editor/master/DotNet"
	objdir "%{StrypeDirectory}/Strype-Editor/master/DotNet/Intermediates"

	links {
		"Coral.Managed"
	}

	propertytags {
		{ "AppendTargetFrameworkToOutputPath", "false" },
		{ "Nullable", "enable" },
	}

	files {
		"%{StrypeDirectory}/Strype-ScriptCore/Source/**.cs",
	}