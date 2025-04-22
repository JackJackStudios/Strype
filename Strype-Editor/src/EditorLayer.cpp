#include "EditorLayer.h"


namespace Strype {

	EditorLayer::EditorLayer()
	{
		m_Room = CreateRef<Room>();
		m_Framebuffer = AGI::Framebuffer::Create(1280, 720);

		//Configure PanelManager
		m_PanelManager.SetRoomContext(m_Room);

		m_PanelManager.AddPanel<SceneHierachyPanel>();
		m_ContentBrowserPanel = m_PanelManager.AddPanel<ContentBrowserPanel>();

		m_ContentBrowserPanel->SetItemClickCallback(AssetType::Room, [this](const AssetMetadata& metadata)
		{
			OpenRoom(metadata.FilePath);
		});

		m_ContentBrowserPanel->SetItemClickCallback(AssetType::Prefab, [this](const AssetMetadata& metadata)
		{
			m_PanelManager.GetInspector()->SetSelected(Project::GetAsset<Prefab>(metadata.Handle).get());
		});

		m_PanelManager.GetInspector()->AddType<Prefab>(STY_BIND_EVENT_FN(EditorLayer::OnInspectorRender));

		OpenProject(false);
	}

	EditorLayer::~EditorLayer()
	{
		SaveProject();
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
		ImGui::Begin((Project::GetFilePath(m_Room->Handle).filename().string()).c_str());

		Application::Get().GetImGuiLayer()->BlockEvents(!ImGui::IsWindowHovered());

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = glm::vec2(viewportSize.x, viewportSize.y);

		ImGui::Image(m_Framebuffer->GetAttachmentID(), viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				AssetHandle handle = *(AssetHandle*)payload->Data;

				switch (Project::GetAssetType(handle))
				{
					case AssetType::Prefab:
					{
						Object newobj = Object::Copy(Project::GetAsset<Prefab>(handle)->GetObject(), m_Room);
						newobj.AddComponent<PrefabComponent>(handle);
						Project::GetAsset<Prefab>(handle)->ConnectObject(newobj);
						break;
					}
					case AssetType::Sprite:
					{
						Object obj = m_Room->CreateObject();
						obj.AddComponent<Transform>(m_Room->GetCamera().Position);
						obj.AddComponent<SpriteRenderer>(handle);
						break;
					}
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

		if (ImGui::BeginPopupContextWindow())
		{
			AddComponentPopup<SpriteRenderer>(prefab, "Sprite Renderer");
			AddComponentPopup<ScriptComponent>(prefab, "Script Component");
			AddComponentPopup<RigidBodyComponent>(prefab, "Rigidbody Component");

			ImGui::EndPopup();
		}

		changed |= DrawComponent<SpriteRenderer>("Sprite Renderer", prefab, [](Prefab* select, SpriteRenderer& component)
		{
			ImGui::ColorEdit4("Colour", glm::value_ptr(component.Colour));

			if (!component.Texture)
			{
				ImGui::Button("<empty>");
			}
			else
			{
				std::string id = std::format("##{0}{1}", (uint32_t)select->GetObject(), "Sprite Renderer");
				ImGui::ImageButton(id.c_str(), (ImTextureID)Project::GetAsset<Sprite>(component.Texture)->Texture->GetRendererID(), ImVec2{ 32.0f, 32.0f }, { 0, 1 }, { 1, 0 });

				if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					component.Texture = 0;
				}

				ImGui::SameLine();
				ImGui::Text("Texture");
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;

					if (Project::GetAssetType(handle) == AssetType::Sprite)
						component.Texture = handle;
					else
						STY_CORE_WARN("Wrong asset type!");
				}
				ImGui::EndDragDropTarget();
			}
		});

		changed |= DrawComponent<ScriptComponent>("Script Component", prefab, [](Prefab* select, ScriptComponent& component)
		{
			auto& scriptEngine = Project::GetScriptEngine();

			const char* scriptName = scriptEngine->IsValidScript(component.ClassID) ? scriptEngine->GetScriptName(component.ClassID).c_str() : "None";

			if (ImGui::Button(scriptName))
			{
				ImGui::OpenPopup("Script search");
			}

			if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				component.ClassID = 0;
			}

			ImGui::SameLine();
			ImGui::Text("Script Class");

			if (ImGui::BeginPopup("Script search"))
			{
				for (const auto& [id, metadata] : scriptEngine->GetAllScripts())
				{
					const auto& scriptName = metadata.FullName;

					if (ImGui::Selectable(scriptName.c_str(), scriptName == scriptEngine->GetScriptName(component.ClassID)))
					{
						component.ClassID = id;
					}
				}

				ImGui::EndPopup();
			}
		});

		changed |= DrawComponent<RigidBodyComponent>("Rigidbody Component", prefab, [](Prefab* select, RigidBodyComponent& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.Type = (BodyType)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
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
					ScriptEngine::BuildProject(Project::GetActive());

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
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		UI_RoomPanel();

		m_PanelManager.OnImGuiRender();
	}

}