#include "RuntimeSession.hpp"

#include "Strype/Project/Project.hpp"

namespace Strype {

	void RuntimeSession::InitRoom(AssetHandle handle)
	{
		m_Room = Project::GetAsset<Room>(handle);
		m_Room->OnResize(m_Project->GetConfig().ViewportSize);
		m_Room->ToggleRuntime(true);
	}

	void RuntimeSession::OnDetach()
	{
		m_Room->ToggleRuntime(false);
		Project::SetActive(nullptr);
	}

	void RuntimeSession::OnUpdate(float ts)
	{
		m_Room->OnUpdate(ts);
		m_Room->OnRender(Renderer::GetCurrent());
	}

	void RuntimeSession::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<RoomTransitionEvent>(STY_BIND_EVENT_FN(RuntimeSession::OnRoomTransition));
	}

	void RuntimeSession::OnRoomTransition(RoomTransitionEvent& e)
	{
		if (!Project::GetAssetManager()->IsAssetLoaded(e.GetHandle())) return;
		m_Room->ToggleRuntime(false);

		InitRoom(e.GetHandle());
	}

}