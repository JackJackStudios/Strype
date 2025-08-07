#include "stypch.hpp"
#include "ProjectSettingsPanel.hpp"

namespace Strype {

	ProjectSettingsPanel::ProjectSettingsPanel()
	{
		Title = "Project Settings";
		Closing = true;
	}

	void ProjectSettingsPanel::OnImGuiRender()
	{
		if (UI::DropdownMenu("Viewport"))
		{
			ProjectConfig& config = Project::GetActive()->GetConfig();

			UI::DragIVec2("Viewport size", config.ViewportSize);
			UI::DragIVec2("Window size", config.RuntimeProps.Size);
			UI::EnumOptions("Window mode", config.RuntimeProps.Mode);

			ImGui::Checkbox("Resizable", &config.RuntimeProps.Resizable);
			ImGui::Checkbox("Borderless", &config.RuntimeProps.Borderless);
			ImGui::Checkbox("V-Sync enabled", &config.RuntimeProps.VSync);
			
			ImGui::TreePop();
		}
	}

}