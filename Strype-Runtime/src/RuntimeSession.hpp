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

		void OnAttach()
		{
			m_Project = (Project::GetActive() ? Project::GetActive() : m_Project);
			Project::SetActive(m_Project);

			InitRoom(Project::GetAssetManager()->GetHandle(m_Project->GetConfig().StartRoom));
		}

		void InitRoom(AssetHandle handle);

		void OnDetach() override;
		void OnUpdate(float ts) override;

		void OnEvent(Event& e) override;
		void OnRoomTransition(RoomTransitionEvent& e);
	private:
		Ref<Room> m_Room = nullptr;
		Ref<Project> m_Project;
	};

}