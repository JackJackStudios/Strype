#pragma once

#include "EditorPanel.hpp"

#include <Strype.hpp>

namespace Strype {

	class EditorSession : public Session
	{
	public:
		EditorSession(Ref<Project> project)
			: m_Project(project)
		{
			WindowProps.Title = "Strype-Editor";
			WindowProps.Mode = AGI::WindowMode::Maximized;

			ImGuiEnabled = true;
			DockspaceEnabled = false;
		}

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(float ts) override;
		void OnImGuiRender() override;
		void OnEvent(Event& e) override;

		template<typename TPanel, typename... TArgs>
		Ref<TPanel> AddPanel(TArgs&&... args)
		{
			static_assert(std::is_base_of<EditorPanel, TPanel>::value, "T must inherit from EditorPanel");
			Ref<TPanel> temp = m_EditorPanels.emplace_back(CreateRef<TPanel>(std::forward<TArgs>(args)...));

			temp->m_CurrentRoom = &m_ActiveRoom;
			return temp;
		}

		template<typename TPanel>
		void RemovePanel(Ref<TPanel> panel)
		{
			m_EditorPanels.erase(std::remove(m_EditorPanels.begin(), m_EditorPanels.end(), panel), m_EditorPanels.end());
		}

	private:
		std::vector<Scope<EditorPanel>> m_EditorPanels;
		Ref<Room> m_ActiveRoom;

		Ref<Project> m_Project;
	};

}