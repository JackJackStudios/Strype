#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "EditorCamera.h"
#include "Panels/PanelManager.h"

#include "Panels/SceneHierachyPanel.h"

namespace Strype {
	
	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
			: m_EditorCamera(1280.0f / 720.0f)
		{
			m_Room = CreateRef<Room>();

			m_PanelManager.AddPanel<SceneHierachyPanel>();

			Object obj = m_Room->CreateObject();
			obj.AddComponent<TagComponent>("Test Entity");
			obj.AddComponent<Transform>();
			obj.AddComponent<SpriteRenderer>();

			m_PanelManager.SetRoomContext(m_Room);

			m_Framebuffer = Framebuffer::Create(1280, 720);
		}

		~EditorLayer()
		{
		}

		void OnUpdate(Timestep ts) override
		{
			m_EditorCamera.OnUpdate(ts);

			if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
				(m_Framebuffer->GetWidth() != m_ViewportSize.x || m_Framebuffer->GetHeight() != m_ViewportSize.y))
			{
				m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				m_EditorCamera.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			}

			m_Framebuffer->Bind();
			Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
			Renderer::Clear();
			
			m_Room->OnUpdate(ts, m_EditorCamera.GetCamera());

			m_Framebuffer->Unbind();
		}

		void OnImGuiRender() override
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");

			Application::Get().GetImGuiLayer()->BlockEvents(!ImGui::IsWindowHovered());

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = glm::vec2(viewportSize.x, viewportSize.y);

			uint64_t textureID = m_Framebuffer->GetAttachmentID();
			ImGui::Image(textureID, viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			ImGui::End();
			ImGui::PopStyleVar();

			m_PanelManager.OnImGuiRender();
		}

		void OnEvent(Event& e) override
		{
			m_EditorCamera.OnEvent(e);

			m_PanelManager.OnEvent(e);
		}

	private:
		Ref<Framebuffer> m_Framebuffer;
		EditorCamera m_EditorCamera;
		Ref<Room> m_Room;
		
		PanelManager m_PanelManager;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	};

	class Editor : public Application
	{
	public:
		Editor(const AppConfig& config)
			: Application(config)
		{
			PushLayer(new EditorLayer());
		}

		~Editor()
		{

		}

	};

	Application* CreateApplication()
	{
		AppConfig config;
		config.DockspaceEnabled = true;

		return new Editor(config);
	}

}