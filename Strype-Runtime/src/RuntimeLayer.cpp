#include "RuntimeLayer.h"

namespace Strype {

	RuntimeLayer::RuntimeLayer()
	{
		OpenProject("C:/Users/Jack/Documents/JackJackStudios/Strype/ExampleProject/ExampleProject.sproj");
		m_Room->OnRuntimeStart();
	}

	RuntimeLayer::~RuntimeLayer()
	{
		m_Room->OnRuntimeStop();
	}

	void RuntimeLayer::OnUpdate(Timestep ts)
	{
		m_Room->OnUpdate(ts, m_RuntimeCamera, true);
	}

	void RuntimeLayer::OnImGuiRender()
	{
	}

	void RuntimeLayer::OnEvent(Event& e)
	{
	}

	void RuntimeLayer::OpenProject(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();
		ProjectSerializer serializer(project);
		serializer.Deserialize(path);

		Project::SetActive(project);

		m_Room = Project::GetAsset<Room>(Project::GetAssetHandle(project->GetStartRoom()));
		Project::SetActiveRoom(m_Room);

	}

}