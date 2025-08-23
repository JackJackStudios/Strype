#pragma once

#include <Strype.hpp>
#include <stb_image.h>

namespace Strype {

	static AGI::Texture LoadTexture(const std::filesystem::path& path)
	{
		int width, height, channels;

		stbi_set_flip_vertically_on_load(0);
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

	class LauncherSession : public Session
	{
	public:
		LauncherSession()
		{
			WindowProps.Title = "Strype Engine - Launcher";
			WindowProps.Size = { 853, 480 };
			WindowProps.Resizable = false;

			ImGuiEnabled = true;
            DockspaceEnabled = false;
		}

		~LauncherSession()
		{
			SaveConfig("assets/launcher.yaml");
		}

		void OnAttach() override
		{
			m_ProjectIcon = LoadTexture("assets/icons/ProjectIcon.png");
			LoadConfig("assets/launcher.yaml");
		}

		bool LoadConfig(const std::filesystem::path& filepath);
		void SaveConfig(const std::filesystem::path& filepath);

		void ProjectWidget(Ref<Project> project, float iconSize);

		void OnImGuiRender() override;
	private:
		std::vector<Ref<Project>> m_LoadedProjects;
		AGI::Texture m_ProjectIcon;
	};

}