#include "RuntimePanel.h"

namespace Strype {

	RuntimePanel::RuntimePanel()
	{
		m_RuntimeRoom = CreateRef<Room>();
		m_Camera = CreateRef<EditorCamera>(1280.0f, 720.0f);
		m_Framebuffer = AGI::Framebuffer::Create(1280, 720);
	}

	void RuntimePanel::StartRuntime()
	{
		m_RuntimeRoom->Clear();
		m_EditorRoom->CopyTo(m_RuntimeRoom);

		m_RuntimeRoom->StartRuntime();
	}

	void RuntimePanel::StopRuntime()
	{
		m_RuntimeRoom->StopRuntime();
	}

	void RuntimePanel::OnUpdate(Timestep ts)
	{
		m_Camera->OnUpdate(ts);

		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(m_Framebuffer->GetWidth() != m_ViewportSize.x || m_Framebuffer->GetHeight() != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_Camera->OnResize(m_ViewportSize.x, m_ViewportSize.y);
		}

		m_Framebuffer->Bind();
		Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
		Renderer::Clear();

		m_RuntimeRoom->OnUpdate(ts, m_Camera->GetCamera());

		m_Framebuffer->Unbind();
	}

	void RuntimePanel::OnImGuiRender()
	{
		if (!m_RuntimeRoom->IsRuntime())
			return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Game");

		Application::Get().GetImGuiLayer()->BlockEvents(!ImGui::IsWindowHovered());

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = glm::vec2(viewportSize.x, viewportSize.y);

		ImGui::Image(m_Framebuffer->GetAttachmentID(), viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();
	}

}

