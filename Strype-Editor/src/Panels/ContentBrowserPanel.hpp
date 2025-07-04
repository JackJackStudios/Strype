#pragma once

#include "EditorPanel.hpp"

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

	using ItemClickCallbackFunc = std::function<void(const AssetMetadata&)>;

	class ContentBrowserPanel : public EditorPanel
	{
	public:
		ContentBrowserPanel();

		virtual void OnImGuiRender();
		virtual void OnProjectChanged() { RefreshAssetTree(); }

		virtual void SetInspector(Ref<InspectorPanel> panel) { m_Inspector = panel; }
		void SetItemClickCallback(AssetType type, ItemClickCallbackFunc func) { m_ItemClicksCallbacks[type] = func; }
		void RefreshAssetTree();
	private:
		void RefreshTreeNode(TreeNode& node);
		void OnInspectorRender(Room* select);
		AGI::Texture GetIcon(AssetHandle handle);
	private:

		std::unordered_map<AssetType, ItemClickCallbackFunc> m_ItemClicksCallbacks;
		
		TreeNode m_RootDirectory;
		TreeNode* m_CurrentDirectory;

		bool m_InputActive = false;
		AssetType m_InputType = AssetType::Sprite; //None = folder
		Ref<InspectorPanel> m_Inspector;

		AGI::Texture m_DirectoryIcon;
		AGI::Texture m_FileIcon;
		std::unordered_map<AssetType, AGI::Texture> m_Icons;
	};

}