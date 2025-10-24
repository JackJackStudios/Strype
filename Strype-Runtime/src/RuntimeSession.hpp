#pragma once

#include "Strype.hpp"

namespace Strype {

	class RuntimeSession : public Session
	{
	public:
		RuntimeSession(const std::filesystem::path& filepath)
			: m_Project(Project::LoadFile(filepath))
		{
			WindowProps.Title = m_Project->GetConfig().Name;
		}

		RuntimeSession(Ref<Project> project)
			: m_Project(project)
		{
			WindowProps.Title = m_Project->GetConfig().Name;
		}

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float ts) override;
	private:
		Ref<Room> m_Room;
		Ref<Project> m_Project;
	};

}