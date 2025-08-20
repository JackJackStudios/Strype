#pragma once

#include "EditorPanel.hpp"

namespace Strype {

	struct TreeNode
	{
		TreeNode* Parent = nullptr;
		std::filesystem::path Path;

		std::vector<TreeNode> Children;
		AssetHandle Handle = 0;

		TreeNode& AddNode(std::filesystem::path filepath, AssetHandle handle = 0)
		{
			for (auto& child : Children)
			{
				if (child.Path == filepath)
					return child;
			}

			TreeNode node;
			node.Parent = this;
			node.Path = filepath;
			node.Handle = handle;

			return Children.emplace_back(node);
		}
	};

	using ItemClickCallbackFunc = std::function<void(Ref<Asset>)>;

	class ContentBrowserPanel : public EditorPanel
	{
	public:
		ContentBrowserPanel();

		virtual void OnImGuiRender();
		virtual void OnEvent(Event& event);
		virtual void OnProjectChanged() { RefreshAssetTree(); }

		void SetItemClickCallback(AssetType type, ItemClickCallbackFunc func) { m_ItemClicksCallbacks[type] = func; }
		void RefreshAssetTree();

		std::filesystem::path GetCurrentPath() const { return m_CurrentDirectory->Path; }
	private:
		void OnInspectorRender(Room* select);
		void OnInspectorRender(Object* select);
		std::vector<char> GetDialogMessage(AssetType type);
		void OnAssetsUpdated(Event& e);

		AGI::Texture GetIcon(AssetHandle handle, TexCoords* tx);
		AGI::Texture GetIcon(AssetType type);
	private:
		std::unordered_map<AssetType, ItemClickCallbackFunc> m_ItemClicksCallbacks;
		std::unordered_map<AssetType, AGI::Texture> m_Icons;
		
		AssetType m_InputType = AssetType::None;
		bool m_InputActivated = false;
		char m_InputText[256] = "";

		TreeNode m_RootDirectory;
		TreeNode* m_CurrentDirectory;

		AGI::Texture m_DirectoryIcon;
		AGI::Texture m_FileIcon;
	};

}