#pragma once

#include "Panels/PanelManager.h"

#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHierachyPanel.h"
#include "Panels/RuntimePanel.h"

#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Strype {

    static bool DecomposeTransform(const glm::mat4& transform, glm::vec2& position, float& rotation, glm::vec2& scale)
    {
        glm::vec2 col0 = glm::vec2(transform[0]); // X axis
		glm::vec2 col1 = glm::vec2(transform[1]); // Y axis

		// Extract translation
		position = glm::vec2(transform[3]);

		// Extract scale
		scale.x = glm::length(col0);
		scale.y = glm::length(col1);

		// Normalize the axes to remove scale
		if (scale.x != 0) col0 /= scale.x;
		if (scale.y != 0) col1 /= scale.y;

		// Compute rotation (angle from X axis)
		rotation = atan2(col0.y, col0.x); // in radians

		return true;
    }

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

		void NewProject(const std::filesystem::path& path = std::filesystem::path());
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
		Ref<SceneHierachyPanel> m_SceneHierachyPanel;

		glm::vec2 m_ViewportBounds[2];
	};

}