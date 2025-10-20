#pragma once

#include <imgui/imgui.h>

namespace Strype {

	namespace Utils {

		static ImVec2 ToImVec2(const glm::vec2& vec)
		{
			return { vec.x, vec.y };
		}

		static std::string ReadFile(const std::filesystem::path& filepath)
		{
			std::string result;
			std::ifstream in(filepath, std::ios::in | std::ios::binary);
			if (in)
			{
				in.seekg(0, std::ios::end);
				size_t size = in.tellg();
				if (size != -1)
				{
					result.resize(size);
					in.seekg(0, std::ios::beg);
					in.read(&result[0], size);
					in.close();
				}
				else
				{
					STY_LOG_ERROR("Platform", "Could not read from file \"{}\"", filepath);
				}
			}
			else
			{
				STY_LOG_ERROR("Platform", "Could not open file \"{}\"", filepath);
			}

			return result;
		}

		static void WriteFile(const std::filesystem::path& filepath, const std::string& content)
		{
			std::ofstream out(filepath.string(), std::ios::out | std::ios::binary);
			out << content;
			out.close();
		}

		static void* ShiftPtr(void* origin, uint32_t shift)
		{
			return (uint8_t*)origin + shift;
		}

		static bool IsFileInsideFolder(const std::filesystem::path& file, const std::filesystem::path& folder)
		{
			auto absFolder = std::filesystem::weakly_canonical(folder);
			auto absFile = std::filesystem::weakly_canonical(file);

			// Check that absFile starts with absFolder
			return std::mismatch(absFolder.begin(), absFolder.end(), absFile.begin()).first == absFolder.end();
		}

	};

	class FileDialogs
	{
	public:
		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);
			   
		static std::filesystem::path OpenFolder();
	};

	class PlatformUtils
	{
	public:
		static bool StartProcess(std::string command);
		static void OpenExternally(const std::filesystem::path& path);
	};

}