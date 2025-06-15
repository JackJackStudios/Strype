#include "RuntimePanel.hpp"

namespace Strype {

	RuntimePanel::RuntimePanel(Ref<Room>& room)
	{
		ScriptEngine::BuildProject(Project::GetActive());
		m_Room = CreateRef<Room>();
		
		AGI::FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { AGI::FramebufferTextureFormat::RGBA8 };
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = Renderer::GetContext()->CreateFramebuffer(framebufferSpec);

		m_Room->Clear();
		room->CopyTo(m_Room);

		m_Room->StartRuntime();
	}

	RuntimePanel::~RuntimePanel()
	{
		m_Room->StopRuntime();
	}

	void RuntimePanel::OnUpdate(float ts)
	{
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(m_Framebuffer->GetWidth() != m_ViewportSize.x || m_Framebuffer->GetHeight() != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_Room->OnResize(m_ViewportSize);
		}

		m_Framebuffer->Bind();
		Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
		Renderer::Clear();

		m_Room->OnUpdate(ts);

		m_Framebuffer->Unbind();
	}

	void RuntimePanel::OnImGuiRender()
	{
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

