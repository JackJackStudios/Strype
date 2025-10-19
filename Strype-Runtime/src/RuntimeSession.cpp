#include "RuntimeSession.hpp"

#include "Strype/Project/Project.hpp"

namespace Strype {

	void RuntimeSession::OnAttach()
	{
		Project::SetActive(m_Project);

		m_Room = Project::GetAsset<Room>(Project::GetAssetManager()->GetHandle(m_Project->GetConfig().StartRoom));
		m_Room->OnResize(m_Project->GetConfig().ViewportSize);
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

}