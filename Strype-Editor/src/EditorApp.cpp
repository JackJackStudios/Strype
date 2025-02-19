#include <Strype.h>
#include <Strype/Core/EntryPoint.h>

#include "EditorCamera.h"
#include "Panels/PanelManager.h"

#include "Panels/SceneHierachyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/ViewportPanel.h"

namespace Strype {
	
	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
			: m_EditorCamera(1280.0f, 720.0f)
		{
			m_Room = CreateRef<Room>();

			//Configure PanelManager
			m_PanelManager.SetRoomContext(m_Room);

			m_PanelManager.AddPanel<SceneHierachyPanel>();
			m_PanelManager.AddPanel<ContentBrowserPanel>();
			m_ViewportPanel = m_PanelManager.AddPanel<ViewportPanel>();

			m_ViewportPanel->SetDrawCallback(STY_BIND_EVENT_FN(EditorLayer::DrawViewport));
			
			m_ViewportPanel->SetResizedCallback([this](float width, float height)
			{
				m_EditorCamera.OnResize(width, height);
			});

			OpenProject(Application::Get().GetConfig().StartupProject);
		}

		~EditorLayer()
		{
		}

		void OnUpdate(Timestep ts) override
		{
			m_EditorCamera.OnUpdate(ts);

			m_ViewportPanel->OnUpdate(ts);
		}

		void DrawViewport(Timestep ts)
		{
			Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
			Renderer::Clear();

			m_Room->OnUpdate(ts, m_EditorCamera.GetCamera());
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

					if (ImGui::MenuItem("Save Room As...", "Ctrl+S"))
						SaveRoomAs();

					ImGui::Separator();

					if (ImGui::MenuItem("Exit"))
						Application::Get().Close();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}


			m_PanelManager.OnImGuiRender();
		}

		void NewRoom()
		{
			m_Room = CreateRef<Room>();
			m_PanelManager.SetRoomContext(m_Room);
		}

		void SaveRoomAs(const std::filesystem::path& path = std::filesystem::path())
		{
			std::filesystem::path dialog = path.empty() ? FileDialogs::SaveFile("Strype Room (*.sroom)\0*.sroom\0") : path;

			if (!dialog.empty())
			{
				RoomSerializer serializer(m_Room);
				serializer.Serialize(dialog);
			}
		}

		void OpenRoom(const std::filesystem::path& path = std::filesystem::path())
		{
			std::filesystem::path dialog = path.empty() ? FileDialogs::OpenFile("Strype Room (*.sroom)\0*.sroom\0") : path;

			if (dialog.empty())
				return; // User click off of file dialog

			RoomSerializer serializer(m_Room);
			serializer.Deserialize(dialog);
			m_PanelManager.SetRoomContext(m_Room);
		}

		void NewProject()
		{
			if (Project::GetActive())
			{
				SaveProject();

				m_PanelManager.SetRoomContext(nullptr);
				Project::SetActive(nullptr);
			}

			// NOTE: This function will copy a template project into
			//       the new directory. This is because a Project cannot exist
			//       without a folder or default room (you must deserialize the room yourself)
			Ref<Project> project = Project::New(FileDialogs::OpenFolder());
			Project::SetActive(project);
			m_PanelManager.OnProjectChanged();

			const std::string& startRoom = project->GetConfig().StartRoom;
			if (!startRoom.empty())
				OpenRoom((Project::GetProjectDirectory() / startRoom).string());
		}

		void SaveProject()
		{
			if (!Project::GetActive())
				return;

			Ref<Project> project = Project::GetActive();
			ProjectSerializer serializer(project);
			serializer.Serialize(project->GetConfig().ProjectDirectory + "/" + project->GetConfig().ProjectFileName);
		}

		void OpenProject(const std::filesystem::path& path = std::filesystem::path())
		{
			std::filesystem::path dialog = path.empty() ? FileDialogs::OpenFile("Strype Project (.sproj)\0*.sproj\0") : path;

			if (dialog.empty())
				return; // User click off of file dialog

			if (Project::GetActive())
			{
				SaveProject();

				m_PanelManager.SetRoomContext(nullptr);
				Project::SetActive(nullptr);
			}
			
			Ref<Project> project = CreateRef<Project>();
			ProjectSerializer serializer(project);
			serializer.Deserialize(dialog);

			Project::SetActive(project);
			m_PanelManager.OnProjectChanged();

			const std::string& startRoom = project->GetConfig().StartRoom;
			if (!startRoom.empty())
				OpenRoom((Project::GetProjectDirectory() / startRoom).string());
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

			m_EditorCamera.OnEvent(e);
			m_PanelManager.OnEvent(e);
		}

	private:
		EditorCamera m_EditorCamera;
		Ref<Room> m_Room;
		
		PanelManager m_PanelManager;
		Ref<ViewportPanel> m_ViewportPanel;
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