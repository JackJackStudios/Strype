#include "InspectorPanel.hpp"

namespace Strype {

	InspectorPanel::InspectorPanel()
	{
	}

	void InspectorPanel::OnImGuiRender()
	{
		ImGui::Begin("Inspector");
		if (m_SelectedAsset)
		{
			m_ItemClicksCallbacks[m_SelectedType](m_SelectedAsset);
		}
		ImGui::End();
	}

}