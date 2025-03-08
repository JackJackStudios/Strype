#include "SceneHierachyPanel.h"

#include <glm/gtc/type_ptr.hpp>

namespace Strype {
	
	SceneHierachyPanel::SceneHierachyPanel()
	{
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
		ImVec2 buttonSize = { ImGui::GetFrameHeight(), ImGui::GetFrameHeight() };
		float itemWidth = (ImGui::GetContentRegionAvail().x - 3 * ImGui::GetStyle().ItemInnerSpacing.x) / 3.0f;

		auto DrawButton = [&](const char* label, float& value, const ImVec4& color) {
			ImVec4 hoverColor = ImVec4(
				std::clamp(color.x + 0.1f, 0.0f, 1.0f),
				std::clamp(color.y + 0.1f, 0.0f, 1.0f),
				std::clamp(color.z + 0.1f, 0.0f, 1.0f),
				1.0f
			);

			ImGui::PushStyleColor(ImGuiCol_Button, color);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
			ImGui::PushFont(boldFont);
			if (ImGui::Button(label, buttonSize))
				value = resetValue;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);
		};

		// Push widgets to the right
		float fullWidth = ImGui::GetContentRegionAvail().x;
		float controlWidth = itemWidth * 2 + buttonSize.x * 2 + ImGui::GetStyle().ItemInnerSpacing.x * 2;
		float offset = fullWidth - controlWidth;
		if (offset > 0)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

		DrawButton("X", values.x, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::SameLine();
		ImGui::SetNextItemWidth(itemWidth);
		ImGui::DragFloat("##X", &values.x, 0.1f, -FLT_MAX, FLT_MAX, "%.2f");

		ImGui::SameLine();
		DrawButton("Y", values.y, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::SameLine();
		ImGui::SetNextItemWidth(itemWidth);
		ImGui::DragFloat("##Y", &values.y, 0.1f, -FLT_MAX, FLT_MAX, "%.2f");

		ImGui::PopStyleVar();
		ImGui::EndTable();

		ImGui::PopID();
	}

	void SceneHierachyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_ActiveScene)
		{
			bool popupOpen = false;

			m_ActiveScene->m_Registry.view<TagComponent>().each([&](auto entity, TagComponent& tag) {

				Object temp{ entity, m_ActiveScene.get() };
				bool selected = m_Selection == entity;

				if (ImGui::Selectable(std::format("{0}##{1}", tag.Tag, (uint32_t)temp).c_str(), &selected))
				{
					m_Selection = temp;
					m_Inspector->SetSelected(&m_Selection);
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete Entity"))
					{
						m_ActiveScene->DestroyObject(temp);
						m_Inspector->SetSelected<void>(nullptr);
					}

					popupOpen = true;
					ImGui::EndPopup();
				}
			});

			if (!popupOpen)
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

		m_Inspector->AddType<Object>(STY_BIND_EVENT_FN(SceneHierachyPanel::OnInspectorRender));
	}

	void SceneHierachyPanel::OnInspectorRender(Object* select)
	{	
		Transform& trans = select->GetComponent<Transform>();
		DrawVec2Control("Position", trans.Position);
		DrawVec2Control("Scale", trans.Scale, 1.0f);
		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::DragFloat("##Rotation", &trans.Rotation);
	}

}