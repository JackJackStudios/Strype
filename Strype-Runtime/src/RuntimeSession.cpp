#include "RuntimeSession.hpp"

#include "Strype/Project/Project.hpp"

namespace Strype {

	void RuntimeSession::OnAttach()
	{
		OpenProject(m_ProjectPath);
		m_Room->ToggleRuntime(true);
	}

	RuntimeSession::~RuntimeSession()
	{
		m_Room->ToggleRuntime(false);
	}

	void RuntimeSession::OnUpdate(float ts)
	{
		m_Room->OnUpdate(ts);
		m_Room->OnRender(Renderer::GetCurrent());
	}

	void RuntimeSession::OpenProject(const std::filesystem::path& path)
	{
		Ref<Project> project = Project::LoadFile(path);
		Project::SetActive(project);

		m_Room = Project::GetAsset<Room>(Project::GetAssetManager()->GetHandle(project->GetConfig().StartRoom));

	}

}