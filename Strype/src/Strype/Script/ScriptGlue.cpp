#include "stypch.hpp"
#include "ScriptGlue.hpp"

#include "Strype/Room/Room.hpp"
#include "Strype/Room/RoomInstance.hpp"

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

		bool Room_ObjectExists(InstanceID id)
		{
			return Project::GetActiveRoom()->InstanceExists(id);
		}

		void Room_DestroyObject(InstanceID id)
		{
			return Project::GetActiveRoom()->DestroyInstance(id);
		}

		void Transform_GetPosition(InstanceID id, glm::vec2* outPosition)
		{
			*outPosition = Project::GetActiveRoom()->GetObject(id).Position;
		}											 
													 
		void Transform_SetPosition(InstanceID id, glm::vec2* inPosition)
		{
			Project::GetActiveRoom()->GetObject(id).Position = *inPosition;
		}

		void Transform_GetRotation(InstanceID id, float* outRotation)
		{
			*outRotation = Project::GetActiveRoom()->GetObject(id).Rotation;
		}

		void Transform_SetRotation(InstanceID id, float* inRotation)
		{
			Project::GetActiveRoom()->GetObject(id).Rotation = *inRotation;
		}

		void Transform_GetScale(InstanceID id, glm::vec2* outScale)
		{
			*outScale = Project::GetActiveRoom()->GetObject(id).Scale;
		}

		void Transform_SetScale(InstanceID id, glm::vec2* inScale)
		{
			Project::GetActiveRoom()->GetObject(id).Scale = *inScale;
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