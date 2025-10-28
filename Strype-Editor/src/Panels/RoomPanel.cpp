#include <Strype.hpp>
#include "RoomPanel.hpp"

#include "EditorSession.hpp"

namespace Strype {

	static bool DecomposeTransform(const glm::mat4& transform, glm::vec2& position, float& rotation, glm::vec2& scale)
	{
		glm::vec2 col0 = glm::vec2(transform[0]); // X axis
		glm::vec2 col1 = glm::vec2(transform[1]); // Y axis

		// Extract translation
		position = glm::vec2(transform[3]);

		// Extract scale
		scale.x = glm::length(col0);
		scale.y = glm::length(col1);

		// Normalize the axes to remove scale
		if (scale.x != 0) col0 /= scale.x;
		if (scale.y != 0) col1 /= scale.y;

		// Compute rotation (angle from X axis)
		rotation = glm::degrees(atan2(col0.y, col0.x)); // in radians

		return true;
	}

	RoomPanel::RoomPanel()
	{
		Title = "Room";
		Closing = false;
		ManualControl = true;

		AGI::FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { AGI::FramebufferTextureFormat::RGBA8 };
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = Renderer::GetCurrent()->GetContext()->CreateFramebuffer(framebufferSpec);
	}

	void RoomPanel::OnUpdate(float ts)
	{
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(m_Framebuffer->GetWidth() != viewportSize.x || m_Framebuffer->GetHeight() != viewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			m_CurrentRoom->OnResize(viewportSize);
		}

		m_Framebuffer->Bind();
		Renderer::GetCurrent()->SetClearColour({ 0.1f, 0.1f, 0.1f });
		Renderer::GetCurrent()->BeginFrame();

		if (m_ViewportFocused)
		{
			int controlX = Input::IsKeyDown(KeyCode::D) - Input::IsKeyDown(KeyCode::A);
			int controlY = Input::IsKeyDown(KeyCode::S) - Input::IsKeyDown(KeyCode::W);
			
			Camera& camera = m_CurrentRoom->GetCamera();
			camera.Position.x += controlX * m_CameraSpeed * ts / camera.GetZoomLevel();
			camera.Position.y += controlY * m_CameraSpeed * ts / camera.GetZoomLevel();
		}

		m_CurrentRoom->OnUpdate(ts);
		m_CurrentRoom->OnRender(Renderer::GetCurrent());

		m_Framebuffer->Unbind();
	}

	void RoomPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Room");

		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		ImVec2 viewportSize = { m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y };
		m_ViewportFocused = ImGui::IsWindowFocused();

		ImGui::Image(m_Framebuffer->GetAttachmentID(), viewportSize, { 0, 1 }, { 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void RoomPanel::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(STY_BIND_EVENT_FN(RoomPanel::OnMouseScrolled));
	}

	void RoomPanel::OnMouseScrolled(MouseScrolledEvent& e)
	{
		if (!m_ViewportFocused) return;

		Camera& camera = m_CurrentRoom->GetCamera();
		camera.SetZoomLevel(glm::clamp(camera.GetZoomLevel() + e.GetOffset().y * 0.25f, 0.25f, 5.0f));
	}

};