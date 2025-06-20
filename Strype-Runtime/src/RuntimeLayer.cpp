#include "RuntimeLayer.hpp"

namespace Strype {

	RuntimeLayer::RuntimeLayer(const std::filesystem::path& path)
	{
		OpenProject(path);
		m_Room->StartRuntime();
	}

	RuntimeLayer::~RuntimeLayer()
	{
		m_Room->StopRuntime();
	}

	void RuntimeLayer::OnUpdate(float ts)
	{
		m_Room->OnUpdate(ts);
	}

	void RuntimeLayer::OpenProject(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();
		ProjectSerializer serializer(project);
		serializer.Deserialize(path);

		Project::SetActive(project);
		ScriptEngine::BuildProject(project->GetProjectDirectory());

		m_Room = Project::GetAsset<Room>(Project::GetAssetHandle(project->GetStartRoom()));

	}

}