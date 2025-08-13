#pragma once

#include "EditorPanel.hpp"
#include "Strype/Utils/ImguiUtils.hpp"

#include <AGI/agi.hpp>

namespace Strype {

	class ProjectSettingsPanel : public EditorPanel
	{
	public:
		ProjectSettingsPanel();

		virtual void OnEvent(Event& event);
		virtual void OnImGuiRender();
	private:
		void OnBindingPressed(BindingPressedEvent& e);
		AGI::Texture GetIcon(BindingType type);
	private:
		InputBinding m_Binding = {};
		std::string m_CurrentVerb = "";

		AGI::Texture m_KeyboardIcon;
		AGI::Texture m_GamepadIcon;
		AGI::Texture m_AxisIcon;
		AGI::Texture m_MouseIcon;
	};

}