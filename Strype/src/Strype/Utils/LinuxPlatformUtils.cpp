#include "stypch.hpp"
#include "Strype/Utils/PlatformUtils.hpp"

#include "Strype/Core/Application.hpp"

#include <GLFW/glfw3.h>

#ifdef STY_LINUX

namespace Strype {

	std::filesystem::path FileDialogs::OpenFile(const char* filter)
	{
		return std::filesystem::path();
	}

	std::filesystem::path FileDialogs::SaveFile(const char* filter)
	{
		return std::filesystem::path();
	}

	std::filesystem::path FileDialogs::OpenFolder()
	{
		return std::filesystem::path();
	}

	bool PlatformUtils::StartProcess(std::string command)
	{
		return false;
	}

	void PlatformUtils::OpenExternally(const std::filesystem::path& path)
	{
	}

}
#endif