#include "SceneHierachyPanel.h"

#include <glm/gtc/type_ptr.hpp>

namespace Strype {
	
	SceneHierachyPanel::SceneHierachyPanel()
	{
	}

	template<typename T>
	static void AddComponentPopup(Object& selection, const std::string& entryName) {
		if (!selection.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				selection.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Object entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::Separator();
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
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	static void DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::BeginTable(label.c_str(), 2, ImGuiTableFlags_SizingStretchSame);
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
		ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::TextUnformatted(label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
		float itemWidth = (ImGui::GetContentRegionAvail().x - 3 * ImGui::GetStyle().ItemInnerSpacing.x) / 3.0f;

		auto DrawButton = [&](const char* label, float& value, const ImVec4& color) {
			ImGui::PushStyleColor(ImGuiCol_Button, color);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(color.x + 0.1f, color.y + 0.1f, color.z + 0.1f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
			ImGui::PushFont(boldFont);
			if (ImGui::Button(label, buttonSize))
				value = resetValue;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);
		};

		DrawButton("X", values.x, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::SameLine();
		ImGui::SetNextItemWidth(itemWidth);
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::SameLine();
		DrawButton("Y", values.y, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::SameLine();
		ImGui::SetNextItemWidth(itemWidth);
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::PopStyleVar();
		ImGui::EndTable();

		ImGui::PopID();
	}

	void SceneHierachyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_ActiveScene)
		{
			m_ActiveScene->m_Registry.view<TagComponent>().each([&](auto entity, TagComponent& tag) {

				Object temp{ entity, m_ActiveScene.get() };
				bool selected = m_Selection == entity;

				if (ImGui::Selectable(tag, &selected))
					m_Selection = temp;

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete Entity"))
						m_ActiveScene->DestroyObject(temp);

					ImGui::EndPopup();
				}
			});

			if (!ImGui::IsItemHovered())
			{
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::MenuItem("Create Entity"))
						m_ActiveScene->CreateObject("Untitled");

					ImGui::EndPopup();
				}
			}
		}

		ImGui::End();

		ImGui::Begin("Inspector");

		if (m_Selection)
		{
			if (m_Selection.HasComponent<TagComponent>())
			{
				TagComponent& tag = m_Selection.GetComponent<TagComponent>();
				
				char buffer[256];
				strncpy(buffer, tag, sizeof(buffer));
				if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				{
					tag = std::string(buffer);
				}
			}

			ImGui::SameLine();
			ImGui::PushItemWidth(-1);

			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				AddComponentPopup<Transform>(m_Selection, "Transform");
				AddComponentPopup<SpriteRenderer>(m_Selection, "Sprite Renderer");

				ImGui::EndPopup();
			}

			ImGui::PopItemWidth();

			DrawComponent<Transform>("Transform", m_Selection, [](auto& component)
			{
				DrawVec2Control("Position", component.Position);
				DrawVec2Control("Scale", component.Scale, 1.0f);
			});

			DrawComponent<SpriteRenderer>("Sprite Renderer", m_Selection, [](SpriteRenderer& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Colour));
			});
		}

		ImGui::End();
	}

}