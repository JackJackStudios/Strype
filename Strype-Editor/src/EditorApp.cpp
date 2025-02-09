#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "Panels/Panel.h"

namespace Strype {

	class TestPanel : public Panel
	{
	public:
		TestPanel(const char* name)
			: m_Name(name)
		{
		}

		void OnUpdate(Timestep ts) override
		{
		}

		void OnImGuiRender() override
		{
		}

		void OnEvent(Event& e) override
		{
		}

	public:
		const char* GetName() override { return m_Name; };
		const char* m_Name;
	};

	class ExampleLayer : public Layer
	{
	public:
		ExampleLayer()
			: m_CameraController(1280.0f / 720.0f)
		{
			m_Framebuffer = Framebuffer::Create(1280, 720);

			m_Panels.push_back(new TestPanel("Test Panel"));
		}

		~ExampleLayer()
		{
			for (Panel* panel : m_Panels)
				delete panel;
		}

		void OnUpdate(Timestep ts) override
		{
			m_CameraController.OnUpdate(ts);

			for (Panel* panel : m_Panels)
				panel->OnUpdate(ts);

			if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
				(m_Framebuffer->GetWidth() != m_ViewportSize.x || m_Framebuffer->GetHeight() != m_ViewportSize.y))
			{
				m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			}

			m_Framebuffer->Bind();
			Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
			Renderer::Clear();

			Renderer::BeginScene(m_CameraController.GetCamera());
			{
				Renderer::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
			}

			Renderer::EndScene();
			m_Framebuffer->Unbind();
		}

		void OnImGuiRender() override
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");

			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();

			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint64_t textureID = m_Framebuffer->GetAttachmentID();
			ImGui::Image(textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			ImGui::End();
			ImGui::PopStyleVar();

			for (Panel* panel : m_Panels)
			{
				ImGui::Begin(panel->GetName());
				panel->OnImGuiRender();
				ImGui::End();
			}
		}

		void OnEvent(Event& e) override
		{
			m_CameraController.OnEvent(e);
		}

	private:
		std::vector<Panel*> m_Panels;

		Ref<Framebuffer> m_Framebuffer;
		CameraController m_CameraController;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	};

	class Sandbox : public Application
	{
	public:
		Sandbox(const AppConfig& config)
			: Application(config)
		{
			PushLayer(new ExampleLayer());
		}

		~Sandbox()
		{

		}

	};

	Application* CreateApplication()
	{
		AppConfig config;
		config.DockspaceEnabled = true;

		return new Sandbox(config);
	}

}