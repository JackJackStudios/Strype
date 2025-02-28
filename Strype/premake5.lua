project "Strype"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

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
        "%{wks.location}/Libraries/imgui/",
        
        "%{wks.location}/Libraries/include",
    }

    links
    {
        "%{wks.location}/Libraries/lib/spdlogd.lib",
        "%{wks.location}/Libraries/lib/glad.lib",
        "%{wks.location}/Libraries/lib/OpenAL32.lib",
        "%{wks.location}/Libraries/lib/sndfile.lib",
        
        "GLFW",
        "ImGui",
        "yaml-cpp",

        "opengl32.lib"
    }

    filter "configurations:Debug"
        defines "STY_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "STY_RELEASE"
        runtime "Release"
        symbols "on"