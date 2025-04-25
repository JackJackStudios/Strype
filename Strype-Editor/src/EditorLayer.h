#pragma once

#include "Panels/PanelManager.h"

#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHierachyPanel.h"
#include "Panels/RuntimePanel.h"

#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	template<typename T>
	static void AddComponentPopup(Prefab* selection, const std::string& entryName) {
		if (!selection->HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				selection->AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

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

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer();

		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;

		void OnEvent(Event& e) override;

		//void NewRoom();
		void OpenRoom(const std::filesystem::path& path);

		void NewProject();
		void OpenProject(bool buildProject, const std::filesystem::path& path = std::filesystem::path());
		void SaveProject();

		void OnInspectorRender(Prefab* prefab);

		void UI_RoomPanel();
	private:
		Ref<AGI::Framebuffer> m_Framebuffer;
		Ref<Room> m_Room;

		PanelManager m_PanelManager;
		Ref<ContentBrowserPanel> m_ContentBrowserPanel;
		Ref<RuntimePanel> m_RuntimePanel;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	};

}