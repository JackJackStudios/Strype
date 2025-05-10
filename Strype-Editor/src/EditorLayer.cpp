#include "EditorLayer.h"

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
		});

		m_ContentBrowserPanel->SetItemClickCallback(AssetType::Prefab, [this](const AssetMetadata& metadata)
		{
			m_PanelManager.GetInspector()->SetSelected(metadata.asset.get());
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

		m_PanelManager.OnUpdate(ts);
	}

	void EditorLayer::UI_RoomPanel()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Room");
		
		Application::Get().GetImGuiLayer()->BlockEvents(!ImGui::IsWindowHovered());

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = glm::vec2(viewportSize.x, viewportSize.y);

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

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::OpenRoom(const std::filesystem::path& path)
	{
		m_Room = Project::GetAsset<Room>(Project::GetAssetHandle(path));
		m_PanelManager.SetRoomContext(m_Room);
	}

	void EditorLayer::NewProject()
	{
		std::filesystem::path path = FileDialogs::OpenFolder();
		if (path.empty())
			return;

		// NOTE: This function will copy a template project into
		//       the new directory. This is because a Project cannot exist
		//       without a folder or default room (you must deserialize the project yourself)
		Project::GenerateNew(path);
		OpenProject(true, path / (path.filename().string() + ".sproj"));
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
		bool changed = false;
		auto& scriptEngine = Project::GetScriptEngine();

		if (SpriteRenderer* spr = prefab->TryGetComponent<SpriteRenderer>())
		{
			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 128.0f) * 0.5f);
			ImGui::Image((ImTextureID)Project::GetAsset<Sprite>(spr->Texture)->Texture->GetRendererID(), ImVec2(128.0f, 128.0f), { 0, 1 }, { 1, 0 });

			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 128.0f) * 0.5f);
			ImGui::Button(Project::GetFilePath(spr->Texture).filename().string().c_str(), ImVec2(128.0f, 0));

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				prefab->RemoveComponent<SpriteRenderer>();
		}
		else
		{
			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 128.0f) * 0.5f);
			ImGui::SetCursorPosY(128.0f);

			ImGui::Button("<empty>", ImVec2(128.0f, 0));
		}

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

		DropdownMenu("Properties", [&]() 
		{
			ImGui::Columns(2, nullptr, false);

			bool solid = prefab->HasComponent<ColliderComponent>();
			bool physics = prefab->HasComponent<RigidBodyComponent>();

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

		if (changed)
		{
			for (const auto& obj : prefab->GetConnectedObjects())
			{
				Object::CopyInto(prefab->GetObject(), obj);
			}
		}
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_PanelManager.OnEvent(e);
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
					m_RuntimePanel = m_PanelManager.AddPanel<RuntimePanel>(m_Room);
				
				if (ImGui::MenuItem("Stop Runtime", ""))
				{
					m_PanelManager.RemovePanel(m_RuntimePanel);
					m_RuntimePanel = nullptr;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		UI_RoomPanel();

		m_PanelManager.OnImGuiRender();
	}

}