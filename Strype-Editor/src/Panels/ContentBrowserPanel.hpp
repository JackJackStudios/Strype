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
		std::vector<char> GetDialogMessage(AssetType type);

		AGI::Texture GetIcon(AssetHandle handle);
		AGI::Texture GetIcon(AssetType type);
	private:
		std::unordered_map<AssetType, ItemClickCallbackFunc> m_ItemClicksCallbacks;
		std::unordered_map<AssetType, AGI::Texture> m_Icons;
		
		AssetType m_InputType = AssetType::None;
		bool m_InputActivated = false;
		char m_InputText[256] = "";

		TreeNode m_RootDirectory;
		TreeNode* m_CurrentDirectory;
		Ref<InspectorPanel> m_Inspector;

		AGI::Texture m_DirectoryIcon;
		AGI::Texture m_FileIcon;
	};

}