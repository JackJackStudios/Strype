#pragma once

#include "Strype.hpp"

namespace Strype {

	class RuntimeSession : public Session
	{
	public:
		~RuntimeSession();
		RuntimeSession(const std::filesystem::path& path)
		{
			m_Project = Project::LoadFile(path);
			WindowProps.Title = m_Project->GetConfig().Name;
		}

		void OnAttach() override;
		void OnUpdate(float ts) override;
	private:
		Ref<Room> m_Room;
		Ref<Project> m_Project;
	};

}