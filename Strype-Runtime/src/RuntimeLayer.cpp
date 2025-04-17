#include "RuntimeLayer.h"

namespace Strype {

	RuntimeLayer::RuntimeLayer()
	{
		OpenProject(FileDialogs::OpenFile("Strype Project (.sproj)\0*.sproj\0"));
		m_Room->StartRuntime();
	}

	RuntimeLayer::~RuntimeLayer()
	{
		m_Room->StopRuntime();
	}

	void RuntimeLayer::OnUpdate(Timestep ts)
	{
		m_Room->OnUpdate(ts, m_RuntimeCamera);
	}

	void RuntimeLayer::OpenProject(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();
		ProjectSerializer serializer(project);
		serializer.Deserialize(path);

		Project::SetActive(project);

		m_Room = Project::GetAsset<Room>(Project::GetAssetHandle(project->GetStartRoom()));

	}

}