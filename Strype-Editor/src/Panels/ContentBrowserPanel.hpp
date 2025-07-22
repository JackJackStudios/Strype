#pragma once

#include "EditorPanel.hpp"

namespace Strype {

	namespace Utils {

		template<typename UIFunction>
		static void DropdownMenu(const std::string& name, UIFunction uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
				ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
				ImGuiTreeNodeFlags_FramePadding;

			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			bool open = ImGui::TreeNodeEx(name.c_str(), treeNodeFlags, "%s", name.c_str());
			ImGui::PopStyleVar();

			if (open)
			{
				uiFunction();
				ImGui::TreePop();
			}
		}

	}

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
		std::vector<char> GetDialogMessage(AssetType type);
		bool OnAssetsUpdated(Event& e);

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