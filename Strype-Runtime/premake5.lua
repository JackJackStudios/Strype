project "Strype-Runtime"
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
        "%{wks.location}/Libraries/Coral/Coral.Native/Include",
        "%{wks.location}/Libraries/box2d/include",
        "%{wks.location}/Libraries/spdlog/include",
        "%{wks.location}/Libraries/glm",
        "%{wks.location}/Libraries/entt/src",
	}

	links
	{
		"Strype"
	}