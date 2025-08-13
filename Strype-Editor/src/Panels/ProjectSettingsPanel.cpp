#include "stypch.hpp"
#include "ProjectSettingsPanel.hpp"

#include <stb_image.h>

namespace Strype {

	static AGI::Texture LoadTexture(const std::filesystem::path& path)
	{
		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

		STY_VERIFY(data, "Failed to load texture \"{}\" ", path);

		AGI::TextureSpecification textureSpec;
		textureSpec.Width = width;
		textureSpec.Height = height;
		textureSpec.Format = AGI::Utils::ChannelsToImageFormat(channels);
		textureSpec.LinearFiltering = true;

		AGI::Texture texture = Renderer::GetCurrent()->GetContext()->CreateTexture(textureSpec);
		texture->SetData(data, width * height * channels);

		stbi_image_free(data);
		return texture;
	}

	ProjectSettingsPanel::ProjectSettingsPanel()
	{
		Title = "Project Settings";
		Closing = true;

		m_KeyboardIcon = LoadTexture("assets/icons/KeyIcon.png");
		m_GamepadIcon = LoadTexture("assets/icons/GamepadIcon.png");
		m_AxisIcon = LoadTexture("assets/icons/AxisIcon.png");
		m_MouseIcon = LoadTexture("assets/icons/MouseIcon.png");
	}

	void ProjectSettingsPanel::OnImGuiRender()
	{
		ProjectConfig& config = Project::GetActive()->m_Config;

		if (UI::DropdownMenu("Viewport"))
		{
			UI::DragIVec2("Viewport size", config.ViewportSize);
			UI::DragIVec2("Window size", config.RuntimeProps.Size);
			UI::EnumOptions("Window mode", config.RuntimeProps.Mode);

			ImGui::Checkbox("Resizable", &config.RuntimeProps.Resizable);
			ImGui::Checkbox("Borderless", &config.RuntimeProps.Borderless);
			ImGui::Checkbox("V-Sync enabled", &config.RuntimeProps.VSync);
			
			ImGui::TreePop();
		}

		if (UI::DropdownMenu("Input Map"))
		{
			char inputText[256] = "";
			auto avalible = ImGui::GetContentRegionAvail().x;

			const char* hint = "Press key...";
			const char* popupID = "InputCapture";
			int iconWidth = 16;
		
			ImGui::SetNextItemWidth((avalible / 4) * 3);
			ImGui::InputText("", inputText, sizeof(inputText));
		
			ImGui::SameLine();
			ImGui::Button("Add verb", ImVec2(ImGui::GetContentRegionAvail().x, 0));

			for (auto& [name, bindings] : Project::GetActive()->m_Bindings)
			{
				if (UI::DropdownMenu(name))
				{
					for (auto& bind : bindings)
					{
						std::string tag = fmt::to_string(bind);
						auto pos = tag.find("::");
						
						AGI::Texture icon = GetIcon(magic_enum::enum_cast<BindingType>(tag.substr(0, pos)).value());
						ImGui::Image(icon->GetRendererID(), ImVec2(16, 16), { 0, 1 }, { 1, 0 });

						ImGui::SameLine();
						ImGui::Selectable(tag.substr(pos + 2).c_str(), false);
						if (ImGui::BeginPopupContextItem())
						{
							if (ImGui::MenuItem("Delete"))
								bindings.erase(std::find(bindings.begin(), bindings.end(), bind));

							ImGui::EndPopup();
						}
					}

					if (ImGui::Button("Add binding"))
					{
						m_CurrentVerb = name;
						ImGui::OpenPopup(popupID);
					}

					if (ImGui::BeginPopup(popupID))
					{
						ImGui::Text(hint);

						if (m_CurrentVerb.empty()) ImGui::CloseCurrentPopup();
						ImGui::EndPopup();
					}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}

	AGI::Texture ProjectSettingsPanel::GetIcon(BindingType type)
	{
		switch (type)
		{
		case BindingType::Keyboard: return m_KeyboardIcon;
		case BindingType::MouseButton: return m_MouseIcon;
		case BindingType::GamepadButton: return m_GamepadIcon;
		case BindingType::GamepadAxis: return m_AxisIcon;
		}
		
		STY_CORE_VERIFY(false, "Undefined BindingType");
		return nullptr;
	}

	void ProjectSettingsPanel::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<BindingPressedEvent>(STY_BIND_EVENT_FN(ProjectSettingsPanel::OnBindingPressed));
	}

	void ProjectSettingsPanel::OnBindingPressed(BindingPressedEvent& e)
	{
		if (m_CurrentVerb.empty()) return;

		auto& verb = Project::GetActive()->m_Bindings[m_CurrentVerb];
		if (std::find(verb.begin(), verb.end(), e.GetBinding()) == verb.end())
		{
			verb.emplace_back(e.GetBinding());
		}

		m_CurrentVerb = "";
	}

}