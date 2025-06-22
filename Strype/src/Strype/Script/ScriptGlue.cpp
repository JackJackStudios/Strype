#include "stypch.hpp"
#include "ScriptGlue.hpp"

#include "Strype/Room/Room.hpp"
#include "Strype/Room/Object.hpp"

#include "Strype/Project/Project.hpp"

#define STY_ADD_INTERNAL_CALL(icall) coreAssembly.AddInternalCall("Strype.InternalCalls", #icall, (void*)InternalCalls::icall)

namespace Strype {

	void ScriptGlue::RegisterGlue(Coral::ManagedAssembly& coreAssembly)
	{
		RegisterComponentTypes(coreAssembly);
		RegisterInternalCalls(coreAssembly);

		coreAssembly.UploadInternalCalls();
	}

	void ScriptGlue::RegisterComponentTypes(Coral::ManagedAssembly& coreAssembly)
	{
	}

	void ScriptGlue::RegisterInternalCalls(Coral::ManagedAssembly& coreAssembly)
	{
		STY_ADD_INTERNAL_CALL(Room_ObjectExists);
		STY_ADD_INTERNAL_CALL(Room_DestroyObject);
		STY_ADD_INTERNAL_CALL(Transform_GetPosition);
		STY_ADD_INTERNAL_CALL(Transform_SetPosition);
		STY_ADD_INTERNAL_CALL(Transform_GetRotation);
		STY_ADD_INTERNAL_CALL(Transform_SetRotation);
		STY_ADD_INTERNAL_CALL(Transform_GetScale);
		STY_ADD_INTERNAL_CALL(Transform_SetScale);
		STY_ADD_INTERNAL_CALL(Log_LogMessage);
	}

	namespace InternalCalls {

		bool Room_ObjectExists(ObjectID id)
		{
			return Project::GetActiveRoom()->InstanceExists(id);
		}

		void Room_DestroyObject(ObjectID id)
		{
			return Project::GetActiveRoom()->DestroyInstance(id);
		}

		void Transform_GetPosition(ObjectID id, glm::vec2* outPosition)
		{
			*outPosition = Project::GetActiveRoom()->GetObject(id).Transform.Position;
		}											 
													 
		void Transform_SetPosition(ObjectID id, glm::vec2* inPosition)
		{
			Project::GetActiveRoom()->GetObject(id).Transform.Position = *inPosition;
		}

		void Transform_GetRotation(ObjectID id, float* outRotation)
		{
			*outRotation = Project::GetActiveRoom()->GetObject(id).Transform.Rotation;
		}

		void Transform_SetRotation(ObjectID id, float* inRotation)
		{
			Project::GetActiveRoom()->GetObject(id).Transform.Rotation = *inRotation;
		}

		void Transform_GetScale(ObjectID id, glm::vec2* outScale)
		{
			*outScale = Project::GetActiveRoom()->GetObject(id).Transform.Scale;
		}

		void Transform_SetScale(ObjectID id, glm::vec2* inScale)
		{
			Project::GetActiveRoom()->GetObject(id).Transform.Scale = *inScale;
		}

		void Log_LogMessage(LogLevel level, Coral::String fmt)
		{
			std::string message = fmt;
			switch (level)
			{
			case LogLevel::Trace:
				STY_TRACE(message);
				break;
			case LogLevel::Info:
				STY_INFO(message);
				break;
			case LogLevel::Warn:
				STY_WARN(message);
				break;
			case LogLevel::Error:
				STY_ERROR(message);
				break;
			}
			Coral::String::Free(fmt);
		}

	}

}