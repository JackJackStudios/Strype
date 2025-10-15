#include "EditorSession.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	// Session for running runtime for projects 
	// that are already loaded by the editor.
	class EditorRuntime : public Session
	{
	public:
		EditorRuntime(Ref<Project> proj)
			: m_ActiveProject(proj)
		{
			WindowProps = m_ActiveProject->GetConfig().RuntimeProps;
			WindowProps.Title = m_ActiveProject->GetConfig().Name;

			ShareResources = true;
		}

		void OnAttach() override
		{
			m_Room = CreateRef<Room>(*Project::GetAsset<Room>(Project::GetAssetManager()->GetHandle(m_ActiveProject->GetConfig().StartRoom)));
			m_Room->OnResize(m_ActiveProject->GetConfig().ViewportSize);

			m_Room->ToggleRuntime(true);
		}

		~EditorRuntime()
		{
			m_Room->ToggleRuntime(false);
		}

		void OnUpdate(float ts) override
		{
			m_Room->OnUpdate(ts);
			m_Room->OnRender(Renderer::GetCurrent());
		}
	private:
		Ref<Project> m_ActiveProject;
		Ref<Room> m_Room;
	};

	void EditorSession::OnAttach()
	{
		// Configure PanelManager
		m_PanelManager.AddPanel<RoomPanel>();
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

		OpenProject(m_Project);
		if (!Project::GetActive()) exit(0);
	}

	EditorSession::~EditorSession()
	{
		m_FileWatcher.reset();
		SaveProject();
	}

	void EditorSession::OnUpdate(float ts)
	{
		m_PanelManager.OnUpdate(ts);
	}

	void EditorSession::OpenRoom(AssetHandle handle)
	{
		m_PanelManager.SetRoomContext(Project::GetAsset<Room>(handle));
	}

	void EditorSession::NewProject(const std::filesystem::path& path)
	{
		std::filesystem::path dialog = path.empty() ? FileDialogs::OpenFolder() : path;
		if (dialog.empty())
			return;

		OpenProject(Project::GenerateNew(dialog));
	}

	void EditorSession::SaveProject()
	{
		Project::SaveAll(Project::GetActive());
	}

	void EditorSession::OpenProject(const std::filesystem::path& path)
	{
		std::filesystem::path dialog = path.empty() ? FileDialogs::OpenFile("Strype Project (.sproj)\0*.sproj\0") : path;

		if (dialog.empty())
			return; // User click off of file dialog

		Ref<Project> project = Project::LoadFile(dialog);

		Project::RestoreCSharp(project);
		OpenProject(project);
	}

	void EditorSession::OpenProject(Ref<Project> project)
	{
		if (Project::GetActive())
			SaveProject();
		
		Project::SetActive(project);
		m_PanelManager.OnProjectChanged();

		m_FileWatcher.reset();
		m_FileWatcher = CreateRef<filewatch::FileWatch<std::string>>(Project::GetProjectDirectory().string(), STY_BIND_EVENT_FN(EditorSession::OnFilewatcher));
		
		OpenRoom(Project::GetAssetManager()->GetHandle(project->GetConfig().StartRoom));
	}

	void EditorSession::OnFilewatcher(const std::filesystem::path& filepath, const filewatch::Event event)
	{
		if (Utils::IsFileInsideFolder(filepath, Project::HiddenFolder))
			return;

		if (event == filewatch::Event::added)
		{
			// Full build - pause editor
			if (AssetManager::GetAssetType(filepath.extension()) == AssetType::Script)
			{
				Project::BuildCSharp(Project::GetActive(), false);
				Project::GetActive()->GetScriptEngine()->ReloadAssembly();
			}
		}

		if (event == filewatch::Event::modified)
		{
			Ref<AssetManager> manager = Project::GetAssetManager();

			if (manager->IsAssetLoaded(filepath))
				manager->ReloadAsset(manager->GetHandle(AssetManager::CalculateName(filepath)));

			//switch (AssetManager::GetAssetType(filepath.extension())
			//{
			//case AssetType::Script:
			//	// Build on seprate thread - runtime waits
			//	break;
			//}
		}
	}

    void EditorSession::OnEvent(Event& e)
	{
		m_PanelManager.OnEvent(e);

		if (e.GetEventType() == EventType::WindowClose)
		{
			// Closes runtime as well
			Application::Get().DispatchEvent<ApplicationQuitEvent>();
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowDropEvent>(STY_BIND_EVENT_FN(EditorSession::OnWindowDrop));
	}

	void EditorSession::OnWindowDrop(WindowDropEvent& e)
	{
		for (const auto& filepath : e.GetPaths())
		{
			AssetType type = AssetManager::GetAssetType(filepath.extension());

			std::filesystem::path newPath = m_ContentBrowserPanel->GetCurrentPath() / filepath.filename();
			if (!std::filesystem::exists(newPath) && type != AssetType::None)
			{
				std::filesystem::copy_file(filepath, newPath);
				Project::GetAssetManager()->ImportAsset(newPath);
			}
		}
	}

	void EditorSession::OnImGuiRender()
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
					Application::Get().DispatchEvent<ApplicationQuitEvent>();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Settings"))
					m_PanelManager.AddPanel<ProjectSettingsPanel>();

				if (ImGui::MenuItem("Build C# Assembly"))
				{
					Project::BuildCSharp(Project::GetActive());
					Project::GetScriptEngine()->ReloadAssembly();
				}

				if (ImGui::MenuItem("Restore C# Project"))
					Project::RestoreCSharp(Project::GetActive());

				ImGui::Separator();

				if (ImGui::MenuItem("Start Runtime"))
				{
					// TODO: Some kind of timestamp system? (filewatcher)
					//Project::BuildCSharp(Project::GetActive(), false);
					//Project::GetActive()->GetScriptEngine()->ReloadAssembly();

					Project::SaveAll(Project::GetActive());
					Application::Get().NewSession<EditorRuntime>(Project::GetActive());
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_PanelManager.OnImGuiRender();
	}

}