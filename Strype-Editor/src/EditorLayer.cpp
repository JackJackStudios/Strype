#include "EditorLayer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	// Layer for running runtime for projects 
	// that are already loaded by the editor.
	class EditorRuntime : public Layer
	{
	public:
		EditorRuntime(Ref<Project> proj)
			: m_ActiveProject(proj)
		{
			WindowProps = m_ActiveProject->GetConfig().RuntimeProps;
			WindowProps.Title = m_ActiveProject->GetConfig().Name;

			Project::BuildCSharp(m_ActiveProject, false);
		}

		void OnAttach() override
		{
			m_Room = Project::GetAsset<Room>(Project::GetHandle(m_ActiveProject->GetConfig().StartRoom))->CopyTo();
			m_Room->OnResize(m_ActiveProject->GetConfig().ViewportSize);

			m_Room->StartRuntime();
		}

		~EditorRuntime()
		{
			m_Room->StopRuntime();
		}

		void OnUpdate(float ts) override
		{
			m_Room->OnUpdate(ts);
		}
	private:
		Ref<Project> m_ActiveProject;
		Ref<Room> m_Room;
	};

	void EditorLayer::OnAttach()
	{
		m_Room = CreateRef<Room>();

		AGI::FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { AGI::FramebufferTextureFormat::RGBA8, AGI::FramebufferTextureFormat::RED_FLOAT };
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = Render->GetContext()->CreateFramebuffer(framebufferSpec);
		
		// Configure PanelManager
		m_PanelManager.SetRoomContext(m_Room);

		m_ContentBrowserPanel = m_PanelManager.AddPanel<ContentBrowserPanel>();

		m_ContentBrowserPanel->SetItemClickCallback(AssetType::Room, [this](Ref<Asset> asset)
		{
			OpenRoom(asset->Handle);
			m_PanelManager.GetInspector()->SetSelected<Room>(asset->Handle);
		});

		m_ContentBrowserPanel->SetItemClickCallback(AssetType::Object, [this](Ref<Asset> asset)
		{
			m_PanelManager.GetInspector()->SetSelected<Object>(asset->Handle);
		});

		m_PanelManager.GetInspector()->AddType<Object>(STY_BIND_EVENT_FN(EditorLayer::OnInspectorRender));

		OpenProject(m_ProjectPath);
		if (!Project::GetActive()) exit(0);
	}

	EditorLayer::~EditorLayer()
	{
		m_FileWatcher.reset();
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
		Render->SetClearColour({ 0.1f, 0.1f, 0.1f });
		Render->Clear();

		m_Room->OnUpdate(ts);
		
		if (Input::IsMouseButtonPressed(MouseCode::Left) && mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			float pixelData = *(float*)m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			InstanceID id = pixelData - 1;

			if (pixelData == 0) m_Selected = -1;
			if (pixelData != 0 && m_Room->InstanceExists(id))
			{
				m_Selected = id;
				m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}
		}
		
		m_Framebuffer->Unbind();

		m_PanelManager.OnUpdate(ts);
	}

	void EditorLayer::UI_RoomPanel()
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
		
				if (Project::GetAssetType(handle) == AssetType::Object)
					m_Room->InstantiatePrefab(handle);
			}
			ImGui::EndDragDropTarget();
		}

		if (m_Room->InstanceExists(m_Selected))
		{
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			RoomInstance& obj = m_Room->GetObject(m_Selected);
			glm::mat4 transform = obj.GetTransform();

			ImGuizmo::Manipulate(
				glm::value_ptr(m_Room->GetCamera().GetViewMatrix()), 
				glm::value_ptr(m_Room->GetCamera().GetProjectionMatrix()),
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

	void EditorLayer::OpenRoom(AssetHandle handle)
	{
		m_Room = Project::GetAsset<Room>(handle);
		m_PanelManager.SetRoomContext(m_Room);
	}

	void EditorLayer::NewProject(const std::filesystem::path& path)
	{
		std::filesystem::path dialog = path.empty() ? FileDialogs::OpenFolder() : path;
		if (dialog.empty())
			return;

		OpenProject(Project::GenerateNew(dialog));
	}

	void EditorLayer::SaveProject()
	{
		Project::SaveFile(Project::GetActive());
		Project::SaveAllAssets();
	}

	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		std::filesystem::path dialog = path.empty() ? FileDialogs::OpenFile("Strype Project (.sproj)\0*.sproj\0") : path;

		if (dialog.empty())
			return; // User click off of file dialog

		Ref<Project> project = Project::LoadFile(dialog);

		Project::RestoreCSharp(project);
		OpenProject(project);
	}

	void EditorLayer::OpenProject(Ref<Project> project)
	{
		if (Project::GetActive())
			SaveProject();
		
		Project::SetActive(project);
		m_PanelManager.OnProjectChanged();

		m_FileWatcher.reset();
		m_FileWatcher = CreateRef<filewatch::FileWatch<std::string>>(Project::GetProjectDirectory().string(), STY_BIND_EVENT_FN(EditorLayer::OnFilewatcher));
		
		OpenRoom(Project::GetHandle(project->GetConfig().StartRoom));
	}

	void EditorLayer::OnInspectorRender(Object* select)
	{
		ImVec2 size = ImVec2(128.0f, 128.0f);
		Ref<Sprite> sprite = Project::GetAsset<Sprite>(select->TextureHandle);
		TexCoords tx = Utils::FlipTexCoordsV(sprite->GetTexCoords());

		UI::CenterWidget(size);
		ImGui::Image(Renderer::GetCurrent()->GetTexture(sprite)->GetRendererID(), size, Utils::ToImVec2(tx[0]), Utils::ToImVec2(tx[2]));

		UI::CenterWidget(size);
		ImGui::Button(select->Name.c_str(), ImVec2(size.x, 0));

		if (UI::DropdownMenu("Properties"))
		{
			float padding = 32.0f;
			ImVec2 child_size(ImGui::GetContentRegionAvail().x - padding*2, 75.0f);
			
			if (ImGui::Button("Add Script")) ImGui::OpenPopup("SearchScript");
			if (ImGui::BeginPopup("SearchScript"))
			{
				auto& scriptEngine = Project::GetScriptEngine();
				for (const auto& [scriptID, metadata] : scriptEngine->GetAllScripts())
				{
					if (!StringMatchingSearch(metadata.FullName, ""))
						continue;

					ImGui::BeginDisabled(std::find(select->Scripts.begin(), select->Scripts.end(), scriptID) != select->Scripts.end());
					if (ImGui::MenuItem(metadata.FullName.c_str()))
						select->Scripts.emplace_back(scriptID);

					ImGui::EndDisabled();
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;

					if (Project::GetAssetType(handle) == AssetType::Script)
					{
						Ref<Script> script = Project::GetAsset<Script>(handle);
						if (Project::GetScriptEngine()->IsValidScript(script->GetID()))
							select->Scripts.emplace_back(script->GetID());
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginChild("ScriptWindow", child_size, ImGuiChildFlags_Borders))
			{
				auto& scriptEngine = Project::GetScriptEngine();

				for (const auto& script : select->Scripts)
				{
					ImGui::Selectable(scriptEngine->GetScriptName(script).c_str(), false);
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete"))
							select->Scripts.erase(std::find(select->Scripts.begin(), select->Scripts.end(), script));

						ImGui::EndPopup();
					}
				}
			}
			ImGui::EndChild();


			ImGui::TreePop();
		}
	}

	void EditorLayer::OnFilewatcher(const std::filesystem::path& filepath, const filewatch::Event event)
	{
		if (event == filewatch::Event::modified)
		{
			Project::ReloadAsset(Project::GetHandle(filepath.stem().string()));
		}
	}

    void EditorLayer::OnEvent(Event& e)
	{
		m_PanelManager.OnEvent(e);
		m_Room->OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowDropEvent>(STY_BIND_EVENT_FN(EditorLayer::OnWindowDrop));
	}

	void EditorLayer::OnWindowDrop(WindowDropEvent& e)
	{
		for (const auto& filepath : e.GetPaths())
		{
			const auto& ext = filepath.extension();
			AssetType type = Utils::GetAssetTypeFromFileExtension(ext);

			std::filesystem::path newPath = m_ContentBrowserPanel->GetCurrentPath() / filepath.filename();
			if (!std::filesystem::exists(newPath) && type != AssetType::None)
			{
				std::filesystem::copy_file(filepath, newPath);
				Project::ImportAsset(newPath);
			}
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project"))
					NewProject();

				if (ImGui::MenuItem("Open Project"))
					OpenProject();

				if (ImGui::MenuItem("Save Project"))
					SaveProject();

				if (ImGui::MenuItem("Exit"))
				{
					ApplicationQuitEvent e;
					Application::Get().OnEvent(e);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Settings"))
					m_PanelManager.AddPanel<ProjectSettingsPanel>();

				if (ImGui::MenuItem("Build C# Assembly"))
					Project::BuildCSharp(Project::GetActive());

				if (ImGui::MenuItem("Restore C# Project"))
					Project::RestoreCSharp(Project::GetActive());

				ImGui::Separator();

				if (ImGui::MenuItem("Start Runtime"))
					Application::Get().PushLayer<EditorRuntime>(Project::GetActive());

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		UI_RoomPanel();

		m_PanelManager.OnImGuiRender();
	}

}