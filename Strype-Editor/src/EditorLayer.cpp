#include "EditorLayer.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>

namespace Strype {

	EditorLayer::EditorLayer(const std::filesystem::path& projectPath)
	{
		m_Room = CreateRef<Room>();

		AGI::FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { AGI::FramebufferTextureFormat::RGBA8 };
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = Renderer::GetContext()->CreateFramebuffer(framebufferSpec);
		
		//Configure PanelManager
		m_PanelManager.SetRoomContext(m_Room);

		m_SceneHierachyPanel = m_PanelManager.AddPanel<SceneHierachyPanel>();
		m_ContentBrowserPanel = m_PanelManager.AddPanel<ContentBrowserPanel>();

		m_ContentBrowserPanel->SetItemClickCallback(AssetType::Room, [this](const AssetMetadata& metadata)
		{
			OpenRoom(metadata.FilePath);
			m_PanelManager.GetInspector()->SetSelected(Project::GetAsset<Room>(Project::GetAssetHandle(metadata.FilePath)).get());
		});

		m_ContentBrowserPanel->SetItemClickCallback(AssetType::Prefab, [this](const AssetMetadata& metadata)
		{
			m_PanelManager.GetInspector()->SetSelected(Project::GetAsset<Prefab>(Project::GetAssetHandle(metadata.FilePath)).get());
		});

		m_PanelManager.GetInspector()->AddType<Prefab>(STY_BIND_EVENT_FN(EditorLayer::OnInspectorRender));

		OpenProject(false, projectPath);
	}

	EditorLayer::~EditorLayer()
	{
		SaveProject();

		//INFO: This line must happen! Usually this happens at end of main
		//      besause the project is static, destorying the project also
		//      destroys assets which must happen before Application shutdown
		//      or Windows/VCRuntime gets involved.
		Project::SetActive(nullptr);
	}

	void EditorLayer::OnUpdate(float ts)
	{
		auto[mouseX, mouseY] = ImGui::GetMousePos();
		mouseX -= m_ViewportBounds[0].x;
		mouseY -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		mouseY = viewportSize.y - mouseY;

		if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(m_Framebuffer->GetWidth() != viewportSize.x || m_Framebuffer->GetHeight() != viewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			m_Room->OnResize(viewportSize);
		}

		m_Framebuffer->Bind();
		Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
		Renderer::Clear();
		
		m_Room->OnUpdate(ts);
		
		//if (Input::IsMouseButtonPressed(MouseCode::ButtonLeft) && mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		//{
		//	int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY) >> 29;
		//	Object obj = Object((entt::entity)pixelData, m_Room.get());
		//
		//	if (pixelData != -1 && obj.IsValid())
		//		m_SceneHierachyPanel->SetSelected(obj);
		//	else
		//		m_SceneHierachyPanel->RemoveSelected();
		//}

		m_Framebuffer->Unbind();

		m_PanelManager.OnUpdate(ts);
	}

	void EditorLayer::UI_RoomPanel()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Room");

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

				if (Project::GetAssetType(handle) == AssetType::Prefab)
					m_Room->InstantiatePrefab(handle);
			}
			ImGui::EndDragDropTarget();
		}

		if (ObjectID selected = m_SceneHierachyPanel->GetSelected())
		{
			ImGuizmo::SetOrthographic(true);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			Transform& t = m_Room->GetObject(selected).Transform;
			glm::mat4 transform = t.GetTransform();

			ImGuizmo::Manipulate(glm::value_ptr(m_Room->GetCamera().GetViewMatrix()), glm::value_ptr(m_Room->GetCamera().GetProjectionMatrix()),
				ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec2 position, scale;
				float rotation;
				DecomposeTransform(transform, position, rotation, scale);

				t.Position = position;
				t.Rotation = rotation;
				t.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::OpenRoom(const std::filesystem::path& path)
	{
		m_Room = Project::GetAsset<Room>(Project::GetAssetHandle(path));
		m_PanelManager.SetRoomContext(m_Room);
	}

	void EditorLayer::NewProject(const std::filesystem::path& path)
	{
		std::filesystem::path dialog = path.empty() ? FileDialogs::OpenFolder() : path;
		if (dialog.empty())
			return;

		// NOTE: This function will copy a template project into
		//       the new directory. This is because a Project cannot exist
		//       without a folder or default room (you must deserialize the project yourself)
		Project::GenerateNew(dialog);
		OpenProject(true, dialog / (dialog.filename().string() + ".sproj"));
	}

	void EditorLayer::SaveProject()
	{
		ProjectSerializer serializer(Project::GetActive());
		serializer.Serialize(Project::GetProjectDirectory() / (Project::GetProjectName() + ".sproj"));
		Project::SaveAllAssets();
	}

	void EditorLayer::OpenProject(bool buildProject, const std::filesystem::path& path)
	{
		std::filesystem::path dialog = path.empty() ? FileDialogs::OpenFile("Strype Project (.sproj)\0*.sproj\0") : path;

		if (dialog.empty())
			return; // User click off of file dialog

		if (Project::GetActive())
			SaveProject();

		Ref<Project> project = CreateRef<Project>();
		ProjectSerializer serializer(project);
		serializer.Deserialize(dialog);

		if (buildProject)
			ScriptEngine::BuildProject(project->GetConfig().ProjectDirectory);

		Project::SetActive(project);
		m_PanelManager.OnProjectChanged();

		OpenRoom(project->GetStartRoom());
	}

	void EditorLayer::OnInspectorRender(Prefab* prefab)
	{
		auto& scriptEngine = Project::GetScriptEngine();

		if (Project::IsAssetLoaded(prefab->TextureHandle))
		{
			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 128.0f) * 0.5f);
			ImGui::Image((ImTextureID)Project::GetAsset<Sprite>(prefab->TextureHandle)->GetTexture()->GetRendererID(), ImVec2(128.0f, 128.0f), { 0, 1 }, { 1, 0 });

			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 128.0f) * 0.5f);
			ImGui::Button(Project::GetFilePath(prefab->Handle).filename().string().c_str(), ImVec2(128.0f, 0));
		}

		DropdownMenu("Properties", [&]() 
		{
			if (Project::IsAssetLoaded(prefab->TextureHandle))
			{
				ImGui::Button(Project::GetFilePath(prefab->TextureHandle).filename().string().c_str());

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						AssetHandle handle = *(AssetHandle*)payload->Data;

						if (Project::GetAssetType(handle) == AssetType::Sprite)
							prefab->TextureHandle = handle;
						else
							STY_CORE_WARN("Wrong asset type!");
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();
				ImGui::Text("Sprite");
			}
		});

		DropdownMenu("Components", [&]() 
		{
			if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				STY_CORE_VERIFY(false, "Not implemented");

				ImGui::EndPopup();
			}
		});
	}

    void EditorLayer::OnEvent(Event& e)
	{
		m_PanelManager.OnEvent(e);
		m_Room->OnEvent(e);
	}

	void EditorLayer::OnImGuiRender()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project"))
					NewProject();

				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
					OpenProject(false);

				if (ImGui::MenuItem("Save Project", "Ctrl+Shift+S"))
					SaveProject();

				ImGui::Separator();

				if (ImGui::MenuItem("Build C# Assembly", "Ctrl+B"))
					ScriptEngine::BuildProject(Project::GetActive()->GetProjectDirectory());

				if (ImGui::MenuItem("Build C# Project", ""))
					Project::BuildProjectFiles(Project::GetProjectDirectory());

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Runtime"))
			{
				if (ImGui::MenuItem("Start Runtime", ""))
				{
					SaveProject();

					std::filesystem::path runtimeExe = std::filesystem::path(std::getenv("STRYPE_DIR")) / "Strype" / "master" / "Strype-Runtime.exe";
					std::filesystem::path projectPath = std::filesystem::path(Project::GetProjectDirectory()) / (Project::GetProjectName() + ".sproj");
					
					PlatformUtils::StartProcess(std::format("\"{}\" \"{}\"", runtimeExe.string(), projectPath.string()));
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		UI_RoomPanel();

		m_PanelManager.OnImGuiRender();
	}

}