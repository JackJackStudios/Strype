#pragma once

#include "Panels/PanelManager.h"

#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHierachyPanel.h"
#include "Panels/RuntimePanel.h"

#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Strype {

    static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
    {
        // From glm::decompose in matrix_decompose.inl

        glm::mat4 LocalMatrix(transform);

        if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<float>()))
            return false;

        if (
            glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<float>(0), glm::epsilon<float>()) ||
            glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<float>(0), glm::epsilon<float>()) ||
            glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<float>(0), glm::epsilon<float>()))
        {
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<float>(0);
            LocalMatrix[3][3] = static_cast<float>(1);
        }

        translation = glm::vec3(LocalMatrix[3]);
        LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

		glm::vec3 Row[3], Pdum3;

        for (glm::length_t i = 0; i < 3; ++i)
            for (glm::length_t j = 0; j < 3; ++j)
                Row[i][j] = LocalMatrix[i][j];

        scale.x = glm::length(Row[0]);
        Row[0] = glm::detail::scale(Row[0], static_cast<float>(1));
        scale.y = glm::length(Row[1]);
        Row[1] = glm::detail::scale(Row[1], static_cast<float>(1));
        scale.z = glm::length(Row[2]);
        Row[2] = glm::detail::scale(Row[2], static_cast<float>(1));

        rotation.y = glm::asin(-Row[0][2]);
        if (glm::cos(rotation.y) != 0)
		{
            rotation.x = atan2(Row[1][2], Row[2][2]);
            rotation.z = atan2(Row[0][1], Row[0][0]);
        }
        else 
		{
            rotation.x = atan2(-Row[2][0], Row[1][1]);
            rotation.z = 0;
        }

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
		Ref<SceneHierachyPanel> m_SceneHierachyPanel;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	};

}