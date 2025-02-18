#include "ContentBrowserPanel.h"

namespace Strype {

	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_DirectoryIcon = Texture::Create("assets/icons/DirectoryIcon.png");
		m_FileIcon = Texture::Create("assets/icons/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		std::filesystem::path parentPath = Project::GetActive()->GetProjectDirectory();

		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != parentPath)
		{
			if (ImGui::Button("<-"))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}

		float padding = 16.0f;
		float thumbnailSize = 90.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			if (path.extension() == ".sproj")
				continue;

			auto relativePath = std::filesystem::relative(path, parentPath);
			std::string filenameString = relativePath.filename().string();

			Ref<Texture> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::ImageButton((std::string("##") + filenameString).c_str(), (ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

		}

		ImGui::Columns(1);
		ImGui::End();
	}


}