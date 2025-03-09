project "Strype-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

    includedirs
	{
		"%{wks.location}/Strype/src",
		"%{wks.location}/Libraries/include",

		"%{wks.location}/Libraries/agi/src",
		"%{wks.location}/Libraries/GLFW/include",
		"%{wks.location}/Libraries/imgui",
	}

	links
	{
		"Strype"
	}

    filter "configurations:Debug"
		defines "STY_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "STY_RELEASE"
		runtime "Release"
		optimize "on"