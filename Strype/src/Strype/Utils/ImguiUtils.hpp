#pragma once

#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	namespace UI {

		static bool DropdownMenu(const std::string& name, ImGuiTreeNodeFlags flags = 0)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
				ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
				ImGuiTreeNodeFlags_FramePadding | flags;

			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::PushID(name.c_str());
			bool open = ImGui::TreeNodeEx(name.c_str(), treeNodeFlags, "%s", name.c_str());
			ImGui::PopID();
			ImGui::PopStyleVar();

			return open;
		}

		static void CenterWidget(ImVec2 size)
		{
			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - size.x) * 0.5f);
		}

		static bool DragVec2(const char* name, glm::vec2& vector)
		{
			return ImGui::DragFloat2(name, glm::value_ptr(vector));
		}

		static bool DragIVec2(const char* name, glm::ivec2& vector)
		{
			return ImGui::DragScalarN(name, ImGuiDataType_U32, glm::value_ptr(vector), 2);
		}

		static bool DragUVec2(const char* name, glm::uvec2& vector)
		{
			return ImGui::DragScalarN(name, ImGuiDataType_U32, glm::value_ptr(vector), 2);
		}

		template<typename T>
		static void EnumOptions(const char* name, T& data)
		{
			constexpr auto enumNames = magic_enum::enum_names<T>();
			auto indexOpt = magic_enum::enum_index<T>(data);
			if (!indexOpt.has_value())
				return;

			std::string_view currentName = enumNames[indexOpt.value()];

			if (ImGui::BeginCombo(name, currentName.data()))
			{
				for (const auto& enumName : enumNames)
				{
					bool isSelected = (currentName == enumName);
					if (ImGui::Selectable(enumName.data(), isSelected))
					{
						if (auto newValue = magic_enum::enum_cast<T>(enumName))
							data = newValue.value();
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}

	};

};