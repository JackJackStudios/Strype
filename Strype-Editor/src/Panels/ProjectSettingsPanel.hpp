#pragma once

#include "EditorPanel.hpp"
#include "Strype/Utils/ImguiUtils.hpp"

namespace Strype {

	class ProjectSettingsPanel : public EditorPanel
	{
	public:
		ProjectSettingsPanel();

		virtual void OnImGuiRender();
	};

}