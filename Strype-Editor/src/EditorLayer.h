#pragma once

#include "EditorCamera.h"
#include "Panels/PanelManager.h"

#include "Panels/SceneHierachyPanel.h"
#include "Panels/ContentBrowserPanel.h"

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

	template<typename T, typename UIFunction>
	static bool DrawComponent(const std::string& name, Prefab* entity, UIFunction uiFunction)
	{
		bool changed = false;

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_FramePadding;
		if (entity->HasComponent<T>())
		{
			T& component = entity->GetComponent<T>();
			T oldComponent = entity->GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(T).hash_code()), treeNodeFlags, "%s", name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

			std::string popupID = "ComponentSettings_" + std::to_string(typeid(T).hash_code());
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup(popupID.c_str());
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup(popupID.c_str()))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(entity, component);

				//HACK: This function fails if component contains pointers
				if (std::memcmp(&component, &oldComponent, sizeof(T)) != 0)
					changed = true;

				ImGui::TreePop();
			}

			if (removeComponent)
			{
				entity->RemoveComponent<T>();
				changed = true;
			}
		}

		return changed;
	}

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer();

		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;

		void OnEvent(Event& e) override;
		bool OnWindowDrop(WindowDropEvent& e);

		void NewRoom();
		void SaveRoom();
		void SaveRoomAs(const std::filesystem::path& path = std::filesystem::path());

		void OpenRoom(const AssetMetadata& metadata);
		void OpenRoom(const std::filesystem::path& path);

		void NewProject();
		void SaveProject();

		void OpenProject(const std::filesystem::path& path = std::filesystem::path());

		void OnInspectorRender(Prefab* prefab);
	private:
		std::filesystem::path m_RoomFilePath;

		Ref<AGI::Framebuffer> m_Framebuffer;
		Ref<EditorCamera> m_EditorCamera;
		Ref<Room> m_Room;

		PanelManager m_PanelManager;
		Ref<ContentBrowserPanel> m_ContentBrowserPanel;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		bool m_RuntimePlayed = false;
	};

}