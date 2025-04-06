project "Strype"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
      
    dependson { "Coral.Managed", "Strype-ScriptCore" }

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stypch.h"
	pchsource "src/stypch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"YAML_CPP_STATIC_DEFINE"
	}

    includedirs
    {
        "src",
        "%{wks.location}/Libraries/yaml-cpp/include",
        "%{wks.location}/Libraries/GLFW/include",
        "%{wks.location}/Libraries/agi/src",
        "%{wks.location}/Libraries/imgui/",
        "%{wks.location}/Libraries/Coral/Coral.Native/Include",

        "%{wks.location}/Libraries/include",
    }

    links
    {
        "%{wks.location}/Libraries/lib/spdlogd.lib",
        "%{wks.location}/Libraries/lib/OpenAL32.lib",
        "%{wks.location}/Libraries/lib/sndfile.lib",
        
        "GLFW",
        "ImGui",
        "yaml-cpp",
        "agi",
        "Coral.Native",

        "opengl32.lib"
    }

    
    postbuildcommands { '{COPYFILE} "%{wks.location}/Libraries/Coral/Coral.Managed/Coral.Managed.runtimeconfig.json" "%{wks.location}/Strype-Editor/DotNet/Coral.Managed.runtimeconfig.json"' }

    filter "configurations:Debug"
        defines "STY_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "STY_RELEASE"
        runtime "Release"
        symbols "on"
        