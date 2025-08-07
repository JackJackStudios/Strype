#include "InspectorPanel.hpp"

namespace Strype {

	InspectorPanel::InspectorPanel()
	{
		Title = "Inspector";
	}

	void InspectorPanel::OnImGuiRender()
	{
		if (m_SelectedAsset)
			m_ItemClicksCallbacks[m_SelectedType](m_SelectedAsset);
	}

}