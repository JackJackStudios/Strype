#pragma once

#include "EditorPanel.h"

namespace Strype {

	class ContentBrowserPanel : public EditorPanel
	{
	public:
		ContentBrowserPanel();

		virtual void OnImGuiRender();
		virtual void OnProjectChanged() { m_CurrentDirectory = Project::GetActive()->GetProjectDirectory(); }
	private:
		Ref<Room> m_ActiveScene;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture> m_DirectoryIcon;
		Ref<Texture> m_FileIcon;
	};

}