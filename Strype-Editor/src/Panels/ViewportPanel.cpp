#include "ViewportPanel.h"

namespace Strype {

	ViewportPanel::ViewportPanel()
	{
		m_Framebuffer = Framebuffer::Create(1280, 720);
	}

	void ViewportPanel::OnUpdate(Timestep ts)
	{
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(m_Framebuffer->GetWidth() != m_ViewportSize.x || m_Framebuffer->GetHeight() != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ResizeCallback(m_ViewportSize.x, m_ViewportSize.y);
		}

		m_Framebuffer->Bind();
		m_DrawCallback(ts);
		m_Framebuffer->Unbind();
	}

	void ViewportPanel::OnImGuiRender()
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
	}

}