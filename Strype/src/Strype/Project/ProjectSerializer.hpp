#pragma once

#include "Project.hpp"

namespace Strype {

	class ProjectSerializer
	{
	public:
		ProjectSerializer(const Ref<Project>& project);

		void Serialize(const std::filesystem::path& filepath);
		void Deserialize(const std::filesystem::path& filepath);
	private:
		Ref<Project> m_Project;
	};

}