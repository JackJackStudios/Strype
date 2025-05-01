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
		"YAML_CPP_STATIC_DEFINE",
        "AL_LIBTYPE_STATIC",
	}

    includedirs
    {
        "src",
        "%{wks.location}/Libraries/yaml-cpp/include",
        "%{wks.location}/Libraries/GLFW/include",
        "%{wks.location}/Libraries/agi/include",
        "%{wks.location}/Libraries/imgui/",
        "%{wks.location}/Libraries/Coral/Coral.Native/Include",
        "%{wks.location}/Libraries/box2d/include",
        "%{wks.location}/Libraries/spdlog/include",
        "%{wks.location}/Libraries/glm",
        "%{wks.location}/Libraries/entt/src",
        "%{wks.location}/Libraries/magic_enum/include",

        "%{wks.location}/Libraries/include",
    }

    links
    {
        "%{wks.location}/Libraries/lib/OpenAL32.lib",
        "%{wks.location}/Libraries/lib/sndfile.lib",
        "%{wks.location}/Libraries/lib/spdlog.lib",
        
        "GLFW",
        "ImGui",
        "yaml-cpp",
        "agi",
        "Coral.Native",
        "Box2D",

        "opengl32.lib",
    }
    
    postbuildcommands { 
        '{COPYFILE} "%{wks.location}/Libraries/Coral/Coral.Managed/Coral.Managed.runtimeconfig.json" "%{wks.location}/Strype/master/DotNet/Coral.Managed.runtimeconfig.json"' 
    }
    
    filter "system:windows"
        links { "winmm.lib", "avrt.lib" }