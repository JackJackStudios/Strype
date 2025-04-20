#include "ContentBrowserPanel.h"

namespace Strype {

	namespace Utils {

		static int NumOfFileOrDirs(const std::filesystem::path& path)
		{
			return std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator{});
		}

	}

	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_DirectoryIcon = AGI::Texture::Create("assets/icons/DirectoryIcon.png");
		m_FileIcon = AGI::Texture::Create("assets/icons/FileIcon.png");
		m_RoomIcon = AGI::Texture::Create("assets/icons/RoomIcon.png");
		m_AudioFileIcon = AGI::Texture::Create("assets/icons/AudioFileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");
		
		float padding = 16.0f;
		float thumbnailSize = 90.0f;
		float cellSize = thumbnailSize + padding;
		
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;
		
		ImGui::Columns(columnCount + 1, 0, false);

		if (m_CurrentDirectory->Parent != nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton("<-", (ImTextureID)m_DirectoryIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			ImGui::PopStyleColor();
			
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				m_CurrentDirectory = m_CurrentDirectory->Parent;

			ImGui::TextWrapped("../");

			ImGui::NextColumn();
		}

		for (TreeNode& node : m_CurrentDirectory->Nodes)
		{
			const std::filesystem::path& path = node.Path;
			std::string name = path.filename().string();
			AssetHandle handle = node.Handle;
			bool isDirectory = std::filesystem::is_directory(m_CurrentDirectory->Path / path);
		
			Ref<AGI::Texture> icon;
			if (isDirectory)
			{
				icon = m_DirectoryIcon;
			}
			else
			{
				switch (Project::GetAssetType(handle))
				{
					case AssetType::Sprite:
					{
						icon = Project::GetAsset<Sprite>(handle)->Texture;
						break;
					}
					case AssetType::Room:
					{
						icon = m_RoomIcon;
						break;
					}
					case AssetType::AudioFile:
					{
						icon = m_AudioFileIcon;
						break;
					}
					default:
					{
						icon = m_FileIcon;
						break;
					}
				}
			}
		
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((std::string("##") + name).c_str(), (ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
		
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &node.Handle, sizeof(AssetHandle));
				ImGui::EndDragDropSource();
			}
		
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (isDirectory)
				{
					m_CurrentDirectory = &node;
				}
				else if (AssetType type = Utils::GetAssetTypeFromFileExtension(path.extension()); m_ItemClicksCallbacks.find(type) != m_ItemClicksCallbacks.end())
				{
					AssetMetadata metadata;
					metadata.FilePath = path;
					metadata.Handle = node.Handle;
					m_ItemClicksCallbacks[type](metadata);
				}
			}
		
			ImGui::PopStyleColor();
		
			ImGui::TextWrapped(name.c_str());
		
			ImGui::NextColumn();
		}
		
		if (ImGui::BeginPopupContextWindow())
		{
			ImGui::MenuItem("New Room");
			ImGui::MenuItem("New Prefab");

			ImGui::EndPopup();
		}

		ImGui::Columns(1);
		ImGui::End();
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		m_RootDirectory = TreeNode(Project::GetProjectDirectory(), nullptr);
		m_CurrentDirectory = &m_RootDirectory;
		
		FillTreeNode(m_RootDirectory);
	}

	void ContentBrowserPanel::FillTreeNode(TreeNode& node)
	{
		if (!Utils::NumOfFileOrDirs(Project::GetProjectDirectory() / node.Path))
			return;

		for (const auto& entry : std::filesystem::directory_iterator(Project::GetProjectDirectory() / node.Path))
		{
			std::filesystem::path relativePath = std::filesystem::relative(entry.path(), m_RootDirectory.Path);
			bool isDirectory = entry.is_directory();

			if (!isDirectory && (s_AssetExtensionMap.find(relativePath.extension()) == s_AssetExtensionMap.end()))
				continue; // Not a valid asset

			if (isDirectory)
			{
				if (!(entry.path().filename() == "strype" || entry.path().filename() == ".vs"))
				{
					node.Nodes.emplace_back(relativePath, &node);
					FillTreeNode(node.Nodes.back());
				}
			}
			else
			{
				node.Nodes.emplace_back(relativePath, &node, Project::GetAssetHandle(relativePath));
			}
		}

		std::stable_partition(node.Nodes.begin(), node.Nodes.end(), [this](const TreeNode& node) 
		{
			return std::filesystem::is_directory(Project::GetProjectDirectory() / node.Path);
		});
	}

}