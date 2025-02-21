#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "EditorCamera.h"
#include "Panels/PanelManager.h"

#include "Panels/SceneHierachyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace Strype {
	
	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
		{
			m_Room = CreateRef<Room>();
			m_EditorCamera = CreateRef<EditorCamera>(1280.0f, 720.0f);
			m_Framebuffer = Framebuffer::Create(1280, 720);

			//Configure PanelManager
			m_PanelManager.SetRoomContext(m_Room);

			m_PanelManager.AddPanel<SceneHierachyPanel>();
			m_ContentBrowserPanel = m_PanelManager.AddPanel<ContentBrowserPanel>();

			m_ContentBrowserPanel->SetItemClickCallback(AssetType::Room, [this](const AssetMetadata& metadata) 
			{
				OpenRoom(metadata);
			});

			OpenProject(Application::Get().GetConfig().StartupProject);
		}

		~EditorLayer()
		{
		}

		void OnUpdate(Timestep ts) override
		{
			m_EditorCamera->OnUpdate(ts);

			if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
				(m_Framebuffer->GetWidth() != m_ViewportSize.x || m_Framebuffer->GetHeight() != m_ViewportSize.y))
			{
				m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				m_EditorCamera->OnResize(m_ViewportSize.x, m_ViewportSize.y);
			}

			m_Framebuffer->Bind();
			Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
			Renderer::Clear();

			m_Room->OnUpdate(ts, m_EditorCamera->GetCamera());
			m_Framebuffer->Unbind();
		}

		void OnImGuiRender() override
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Project"))
						NewProject();

					if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
						OpenProject();

					if (ImGui::MenuItem("Save Project", "Ctrl+Shift+S"))
						SaveProject();

					ImGui::Separator();

					if (ImGui::MenuItem("New Room", "Ctrl+N"))
						NewRoom();

					if (ImGui::MenuItem("Save Room", "Ctrl+S"))
						SaveRoom();

					ImGui::Separator();

					if (ImGui::MenuItem("Exit"))
						Application::Get().Close();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");

			Application::Get().GetImGuiLayer()->BlockEvents(!ImGui::IsWindowHovered());

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = glm::vec2(viewportSize.x, viewportSize.y);

			ImGui::Image(m_Framebuffer->GetAttachmentID(), viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;

					if (Project::GetAssetType(handle) == AssetType::Room)
						OpenRoom(Project::GetMetadata(handle));
					else if (Project::GetAssetType(handle) == AssetType::Texture)
					{
						Object obj = m_Room->CreateObject(Project::GetMetadata(handle).FilePath.stem().string());
						obj.AddComponent<Transform>(m_EditorCamera->GetPosition());
						obj.AddComponent<SpriteRenderer>(handle);
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::End();
			ImGui::PopStyleVar();

			m_PanelManager.OnImGuiRender();
		}

		void NewRoom()
		{
			m_Room = CreateRef<Room>();
			m_PanelManager.SetRoomContext(m_Room);
			m_RoomFilePath = std::string();
		}

		void SaveRoom()
		{
			RoomSerializer serializer(m_Room);

			if (!m_RoomFilePath.empty())
				serializer.Serialize(Project::GetProjectDirectory() / m_RoomFilePath);
			else
				SaveRoomAs();
		}

		void SaveRoomAs(const std::filesystem::path& path = std::filesystem::path())
		{
			std::filesystem::path dialog = path.empty() ? FileDialogs::SaveFile("Strype Room (*.sroom)\0*.sroom\0") : path;

			if (!dialog.empty())
			{
				RoomSerializer serializer(m_Room);
				serializer.Serialize(dialog);
				m_RoomFilePath = dialog;
			}
		}

		void OpenRoom(const AssetMetadata& metadata)
		{
			m_Room = Project::GetAsset<Room>(metadata.Handle);
			m_PanelManager.SetRoomContext(m_Room);
			m_RoomFilePath = Project::GetFilePath(metadata.Handle);
		}

		void OpenRoom(const std::filesystem::path& path)
		{
			const AssetMetadata& metadata = Project::GetMetadata(Project::GetAssetHandle(path));
			OpenRoom(metadata);
		}

		void NewProject()
		{
			if (Project::GetActive())
				SaveProject();

			std::filesystem::path path = FileDialogs::OpenFolder();
			if (path.empty())
				return;

			// NOTE: This function will copy a template project into
			//       the new directory. This is because a Project cannot exist
			//       without a folder or default room (you must deserialize the room yourself)
			Ref<Project> project = Project::New(path);
			Project::SetActive(project);
			m_PanelManager.OnProjectChanged();

			const std::string& startRoom = project->GetConfig().StartRoom;
			if (!startRoom.empty())
				OpenRoom(Project::GetProjectDirectory() / startRoom);
		}

		void SaveProject()
		{
			if (!Project::GetActive())
				return;

			Ref<Project> project = Project::GetActive();
			ProjectSerializer serializer(project);
			serializer.Serialize(project->GetConfig().ProjectDirectory / project->GetConfig().ProjectFileName);
		}

		void OpenProject(const std::filesystem::path& path = std::filesystem::path())
		{
			std::filesystem::path dialog = path.empty() ? FileDialogs::OpenFile("Strype Project (.sproj)\0*.sproj\0") : path;

			if (dialog.empty())
				return; // User click off of file dialog

			if (Project::GetActive())
				SaveProject();
			
			Ref<Project> project = CreateRef<Project>();
			ProjectSerializer serializer(project);
			serializer.Deserialize(dialog);

			Project::SetActive(project);
			m_PanelManager.OnProjectChanged();

			const std::string& startRoom = project->GetConfig().StartRoom;
			if (!startRoom.empty())
				OpenRoom(startRoom);
		}

		bool OnWindowDrop(WindowDropEvent& e)
		{
			const std::filesystem::path& path = e.GetPaths()[0];
			std::string ext = path.extension().string();

			if (ext == ".sproj")
				OpenProject(path);

			return false;
		}

		void OnEvent(Event& e) override
		{
			EventDispatcher dispatcher(e);
			dispatcher.Dispatch<WindowDropEvent>(STY_BIND_EVENT_FN(EditorLayer::OnWindowDrop));

			m_EditorCamera->OnEvent(e);
			m_PanelManager.OnEvent(e);
		}

	private:
		std::filesystem::path m_RoomFilePath;

		Ref<Framebuffer> m_Framebuffer;
		Ref<EditorCamera> m_EditorCamera;
		Ref<Room> m_Room;
		
		PanelManager m_PanelManager;
		Ref<ContentBrowserPanel> m_ContentBrowserPanel;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	};

	class Editor : public Application
	{
	public:
		Editor(const AppConfig& config)
			: Application(config)
		{
			PushLayer(new EditorLayer());
		}

		~Editor()
		{

		}

	};

	Application* CreateApplication()
	{
		AppConfig config;
		config.StartupFrames = 10;
		config.DockspaceEnabled = true;
		config.StartupProject = "C:/Users/Jack/Documents/JackJackStudios/Strype/ExampleProject/ExampleProject.sproj";

		return new Editor(config);
	}

}