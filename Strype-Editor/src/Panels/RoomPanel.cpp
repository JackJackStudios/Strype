#include "RoomPanel.hpp"

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
		Closing = true;

		AGI::FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { AGI::FramebufferTextureFormat::RGBA8 };
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = Renderer::GetCurrent()->GetContext()->CreateFramebuffer(framebufferSpec);
	}

	void RoomPanel::OnUpdate(float ts)
	{
		auto [mouseX, mouseY] = ImGui::GetMousePos();
		mouseX -= m_ViewportBounds[0].x;
		mouseY -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		mouseY = viewportSize.y - mouseY;

		if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(m_Framebuffer->GetWidth() != viewportSize.x || m_Framebuffer->GetHeight() != viewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			m_CurrentRoom->OnResize(viewportSize);
		}

		m_Framebuffer->Bind();
		Renderer::GetCurrent()->SetClearColour({ 0.1f, 0.1f, 0.1f });
		Renderer::GetCurrent()->BeginFrame();

		m_CurrentRoom->OnUpdate(ts);
		m_CurrentRoom->OnRender(Renderer::GetCurrent());

		if (Input::IsMouseButtonPressed(MouseCode::Left))
		{
			bool found = false;

			for (const auto& handle : *m_CurrentRoom)
			{
				RoomInstance& inst = m_CurrentRoom->GetInstance(handle);

				glm::vec2 framebufPos = inst.Position + m_CurrentRoom->GetCamera().GetHalfSize();
				glm::vec2 frameSize = Project::GetAsset<Sprite>(Project::GetAsset<Object>(inst.ObjectHandle)->TextureHandle)->GetFrameSize() * m_CurrentRoom->GetCamera().GetZoomLevel();

				if (PointInRectangle({ mouseX, mouseY }, { framebufPos.x - frameSize.x / 2, framebufPos.y - frameSize.y / 2, frameSize }))
				{
					m_Selected = handle;
					m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;

					found = true;
					break;
				}
			}

			if (!found)
			{
				m_Selected = -1;
			}
		}

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

		ImGui::Image(m_Framebuffer->GetAttachmentID(), viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				AssetHandle handle = *(AssetHandle*)payload->Data;

				if (Project::GetAsset<Asset>(handle)->GetType() == AssetType::Object)
				{
					RoomInstance instance;
					instance.ObjectHandle = handle;
					instance.Position = { 0.0f, 0.0f };
					m_CurrentRoom->CreateInstance(instance);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (m_CurrentRoom->InstanceExists(m_Selected))
		{
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			RoomInstance& obj = m_CurrentRoom->GetInstance(m_Selected);
			glm::mat4 transform = Renderer::GetTransform({ obj.Position.x, obj.Position.y, 0.0f }, obj.Scale, obj.Rotation);

			ImGuizmo::Manipulate(
				glm::value_ptr(m_CurrentRoom->GetCamera().GetViewMatrix()),
				glm::value_ptr(m_CurrentRoom->GetCamera().GetProjectionMatrix()),
				m_GuizmoType,
				ImGuizmo::LOCAL,
				glm::value_ptr(transform),
				nullptr, nullptr
			);

			if (ImGuizmo::IsUsing())
			{
				glm::vec2 position, scale;
				float rotation;
				DecomposeTransform(transform, position, rotation, scale);

				obj.Position = position;
				obj.Rotation = rotation;
				obj.Scale = scale;
			}
			else
			{
				if (Input::IsKeyDown(KeyCode::I))
				{
					m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
				}
				else if (Input::IsKeyDown(KeyCode::O))
				{
					m_GuizmoType = ImGuizmo::OPERATION::SCALE;
				}
				else if (Input::IsKeyDown(KeyCode::P))
				{
					m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
				}
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void RoomPanel::OnEvent(Event& e)
	{
		//m_CurrentRoom->OnEvent(e);
	}

};