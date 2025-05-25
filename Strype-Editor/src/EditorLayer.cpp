#include "EditorLayer.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>

namespace Strype {

	EditorLayer::EditorLayer()
	{
		m_Room = CreateRef<Room>();

		AGI::FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { AGI::FramebufferTextureFormat::RGBA8, AGI::FramebufferTextureFormat::RED_INTEGER };
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = AGI::Framebuffer::Create(framebufferSpec);
		
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

		OpenProject(false);
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

	void EditorLayer::OnUpdate(Timestep ts)
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
		
		m_Framebuffer->ClearAttachment(1, -1);
		m_Room->OnUpdate(ts);
		
		if (Input::IsMouseButtonPressed(MouseCode::ButtonLeft) && mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			// HACK: The object ID is int but should be int
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY) >> 29;
			Object obj = Object((entt::entity)pixelData, m_Room.get());

			if (pixelData != -1 && obj.IsValid())
				m_SceneHierachyPanel->SetSelected(obj);
			else
				m_SceneHierachyPanel->RemoveSelected();
		}

		m_Framebuffer->Unbind();

		m_PanelManager.OnUpdate(ts);
	}

	void EditorLayer::UI_RoomPanel()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Room");
		
		Application::Get().GetImGuiLayer()->BlockEvents(!ImGui::IsWindowHovered());

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
				{
					Object newobj = Object::Copy(Project::GetAsset<Prefab>(handle)->GetObject(), m_Room);
					newobj.AddComponent<PrefabComponent>(handle);
					newobj.AddComponent<Transform>(glm::vec2(m_Room->GetCamera().Position));
					Project::GetAsset<Prefab>(handle)->ConnectObject(newobj);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (Object selected = m_SceneHierachyPanel->GetSelected())
		{
			ImGuizmo::SetOrthographic(true);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			auto& tc = selected.GetComponent<Transform>();
			glm::mat4 transform = tc.GetTransform();

			ImGuizmo::Manipulate(glm::value_ptr(m_Room->GetCamera().GetViewMatrix()), glm::value_ptr(m_Room->GetCamera().GetProjectionMatrix()),
				ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec2 position, scale;
				float rotation;
				DecomposeTransform(transform, position, rotation, scale);

				tc.Position = position;
				tc.Rotation = rotation;
				tc.Scale = scale;
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
		//OpenProject(true, path / (path.filename().string() + ".sproj"));
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
			ScriptEngine::BuildProject(project);

		Project::SetActive(project);
		m_PanelManager.OnProjectChanged();

		OpenRoom(project->GetStartRoom());
	}

	void EditorLayer::OnInspectorRender(Prefab* prefab)
	{
		auto& scriptEngine = Project::GetScriptEngine();

		if (SpriteRenderer* spr = prefab->TryGetComponent<SpriteRenderer>())
		{
			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 128.0f) * 0.5f);
			ImGui::Image((ImTextureID)Project::GetAsset<Sprite>(spr->Texture)->GetTexture()->GetRendererID(), ImVec2(128.0f, 128.0f), { 0, 1 }, { 1, 0 });

			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 128.0f) * 0.5f);
			ImGui::Button(Project::GetFilePath(prefab->Handle).filename().string().c_str(), ImVec2(128.0f, 0));
		}

		DropdownMenu("Properties", [&]() 
		{
			bool solid = prefab->HasComponent<ColliderComponent>();
			bool physics = prefab->HasComponent<RigidBodyComponent>();

			if (SpriteRenderer* spr = prefab->TryGetComponent<SpriteRenderer>())
			{
				ImGui::Button(Project::GetFilePath(spr->Texture).filename().string().c_str());

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						AssetHandle handle = *(AssetHandle*)payload->Data;

						if (Project::GetAssetType(handle) == AssetType::Sprite)
							prefab->EnsureCurrent<SpriteRenderer>().Texture = handle;
						else
							STY_CORE_WARN("Wrong asset type!");
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();
				ImGui::Text("Sprite");
			}

			ImGui::Columns(2, nullptr, false);

			if (ImGui::Checkbox("Solid", &solid))
			{
				if (solid)
					prefab->AddComponent<ColliderComponent>(glm::vec2(1.0f, 1.0f));
				else
					prefab->RemoveComponent<ColliderComponent>();
			}
			ImGui::NextColumn();

			if (ImGui::Checkbox("Uses Physics", &physics))
			{
				if (physics)
					prefab->AddComponent<RigidBodyComponent>(BodyType::Kinematic);
				else
					prefab->RemoveComponent<RigidBodyComponent>();
			}
			ImGui::NextColumn();

			ImGui::Columns(1);

		});

		DropdownMenu("Components", [&]() 
		{
			if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				for (const auto& [id, metadata] : scriptEngine->GetAllScripts())
				{
					ScriptContainer& sc = prefab->EnsureCurrent<ScriptContainer>();

					if (std::find(sc.begin(), sc.end(), ScriptComponent(id)) != sc.end())
						continue;

					if (ImGui::MenuItem(metadata.FullName.c_str()))
						sc.emplace_back(id);
				}

				ImGui::EndPopup();
			}

			if (ScriptContainer* sc = prefab->TryGetComponent<ScriptContainer>())
			{
				for (const auto& script : *sc)
				{
					ImGui::MenuItem(scriptEngine->GetScriptName(script.ClassID).c_str());

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Remove component"))
							sc->erase(std::remove(sc->begin(), sc->end(), script), sc->end());

						ImGui::EndPopup();
					}
				}
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
					ScriptEngine::BuildProject(Project::GetActive());

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