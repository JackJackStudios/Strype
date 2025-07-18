StrypeDirectory = os.getenv("STRYPE_DIR")

workspace "EmptyProject"
	startproject "EmptyProject"
	configurations { "Debug", "Release" }

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"
	
	project "EmptyProject"
		kind "SharedLib"
		language "C#"
		dotnetframework "net8.0"

		targetname "EmptyProject"
		targetdir "%{prj.location}/bin"
		objdir "%{prj.location}/bin"

		files  { "../**.cs", }
		removefiles { "./**.cs" }

		links 
		{
			"%{StrypeDirectory}/Strype/master/DotNet/Strype-ScriptCore.dll",
			"%{StrypeDirectory}/Strype/master/DotNet/Coral.Managed.dll",
		}