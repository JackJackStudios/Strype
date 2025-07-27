#pragma once

#include "FileWatch.hpp"
#include "Panels/PanelManager.hpp"
#include "Panels/ContentBrowserPanel.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>

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
		rotation = glm::degrees(atan2(col0.y, col0.x)); // in radians

		return true;
    }

	template<typename UIFunction>
	void DropdownMenu(const std::string& name, UIFunction uiFunction)
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
		~EditorLayer();
		EditorLayer(const std::filesystem::path& projectPath)
			: m_ProjectPath(projectPath) 
		{
			WindowProps.Title = "Strype-Editor";
			WindowProps.Maximise = true;
		}

		void OnAttach() override;
		void OnUpdate(float ts) override;
		void OnImGuiRender() override;

		void OnEvent(Event& e) override;

		void OpenRoom(AssetHandle handle);

		void NewProject(const std::filesystem::path& path = std::filesystem::path());
		void SaveProject();

		void OpenProject(const std::filesystem::path& path = std::filesystem::path());
		void OpenProject(Ref<Project> project);
	private:
		bool OnWindowDrop(WindowDropEvent& e);
		void OnInspectorRender(Object* object);

		void UI_RoomPanel();
		void FilewatcherFunc(const std::string& str, const filewatch::Event event);
	private:
		AGI::Framebuffer m_Framebuffer;
		InstanceID m_Selected;
		Ref<Room> m_Room;
		Ref<filewatch::FileWatch<std::string>> m_FileWatcher;

		PanelManager m_PanelManager;
		Ref<ContentBrowserPanel> m_ContentBrowserPanel;

		glm::vec2 m_ViewportBounds[2];
		ImGuizmo::OPERATION m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
		std::filesystem::path m_ProjectPath;
	};

}