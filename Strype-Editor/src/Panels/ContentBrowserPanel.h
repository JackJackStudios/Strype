#pragma once

#include "EditorPanel.h"

namespace Strype {

	struct TreeNode
	{
		TreeNode* Parent;
		std::filesystem::path Path;
		AssetHandle Handle;
		std::vector<TreeNode> Nodes;

		TreeNode()
			: Path(), Parent(nullptr), Handle(0) {}
		TreeNode(std::filesystem::path path, TreeNode* parent, AssetHandle handle = (uint64_t)0)
			: Path(path), Parent(parent), Handle(handle) {}
	};

	using ItemClickCallbackFunc = std::function<void(const std::filesystem::path&)>;

	class ContentBrowserPanel : public EditorPanel
	{
	public:
		ContentBrowserPanel();

		virtual void OnImGuiRender();
		virtual void OnProjectChanged() { RefreshAssetTree(); }

		void SetItemClickCallback(AssetType type, ItemClickCallbackFunc func) { m_ItemClicksCallbacks[type] = func; }
		void FillTreeNode(TreeNode& node, std::unordered_map<std::filesystem::path, AssetHandle>& assetRegistry);
		void RefreshAssetTree();
	private:
		Ref<Room> m_ActiveScene;
		std::unordered_map<AssetType, ItemClickCallbackFunc> m_ItemClicksCallbacks;
		
		TreeNode m_RootDirectory;
		TreeNode* m_CurrentDirectory;

		Ref<Texture> m_DirectoryIcon;
		Ref<Texture> m_FileIcon;
	};

}