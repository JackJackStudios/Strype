#include "SceneHierachyPanel.h"

namespace Strype {
	
	SceneHierachyPanel::SceneHierachyPanel()
	{
	}

	void SceneHierachyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_ActiveScene)
		{
			m_ActiveScene->m_Registry.view<TagComponent>().each([&](auto entity, TagComponent& tag) {
				
				bool selected = m_Selection == entity;
				if (ImGui::Selectable(tag.Tag.c_str(), &selected))
				{
					Object temp{ entity, m_ActiveScene.get() };
					m_Selection = temp;
				}
			});
		}

		ImGui::End();
	}

}