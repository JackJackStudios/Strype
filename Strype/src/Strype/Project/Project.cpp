#include "stypch.h"
#include "Project.h"

namespace Strype {

	Project::Project()
	{
	}

	Project::~Project()
	{
	}

	void Project::SetActive(Ref<Project> project)
	{
		if (s_ActiveProject)
		{
		}

		s_ActiveProject = project;
	}

}