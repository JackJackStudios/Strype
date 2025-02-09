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
			// ..Shutdown
		}

		// ..Initizalse again
		s_ActiveProject = project;
	}

}