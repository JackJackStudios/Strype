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
		m_DirectoryIcon = Texture::Create("assets/icons/DirectoryIcon.png");
		m_FileIcon = Texture::Create("assets/icons/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory->Parent != nullptr)
		{
			if (ImGui::Button("<-"))
				m_CurrentDirectory = m_CurrentDirectory->Parent;
		}

		float padding = 16.0f;
		float thumbnailSize = 90.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (TreeNode& node : m_CurrentDirectory->Nodes)
		{
			const std::filesystem::path& path = node.Path;
			std::string name = path.filename().string();
			AssetHandle handle = node.Handle;
			bool isDirectory = std::filesystem::is_directory(m_CurrentDirectory->Path / path);
			Ref<Texture> icon = isDirectory ? m_DirectoryIcon : m_FileIcon;

			ImGui::ImageButton((std::string("##") + name).c_str(), (ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (isDirectory)
				{
					m_CurrentDirectory = &node;
				}
				else if (AssetType type = Utils::GetAssetTypeFromFileExtension(path.extension()); m_ItemClicksCallbacks.find(type) != m_ItemClicksCallbacks.end())
				{
					m_ItemClicksCallbacks[type](path);
				}
			}

			ImGui::TextWrapped(name.c_str());

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::End();
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		// Flip values of the Asset Registy so AssetHandle can be
		// associated with TreeNodes
		std::unordered_map<std::filesystem::path, AssetHandle> assetRegistry;

		for (const auto& [handle, metadata] : Project::GetAssetManager()->GetAssetRegistry())
			assetRegistry[metadata.FilePath] = handle;

		m_RootDirectory = TreeNode(Project::GetProjectDirectory(), nullptr);
		m_CurrentDirectory = &m_RootDirectory;

		// Recersivly set AssetHandles to TreeNodes
		FillTreeNode(m_RootDirectory, assetRegistry);
	}

	void ContentBrowserPanel::FillTreeNode(TreeNode& node, std::unordered_map<std::filesystem::path, AssetHandle>& assetRegistry)
	{
		if (!Utils::NumOfFileOrDirs(node.Path))
			return;

		for (const auto& entry : std::filesystem::directory_iterator(node.Path))
		{
			std::filesystem::path relativePath = std::filesystem::relative(entry.path(), m_RootDirectory.Path);
			bool isDirectory = entry.is_directory();

			if (!isDirectory && (s_AssetExtensionMap.find(relativePath.extension()) == s_AssetExtensionMap.end()))
				continue; // Not a valid asset

			if (isDirectory)
			{
				node.Nodes.emplace_back(entry.path(), &node);
				FillTreeNode(node.Nodes.back(), assetRegistry);
			}
			else
			{
				node.Nodes.emplace_back(relativePath, &node, assetRegistry.at(relativePath));
			}
		}
	}


}