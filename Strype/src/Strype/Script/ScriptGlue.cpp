#include "stypch.hpp"
#include "ScriptGlue.hpp"

#include "Strype/Room/Room.hpp"
#include "Strype/Room/RoomInstance.hpp"

#include "Strype/Core/Input.hpp"
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
		STY_ADD_INTERNAL_CALL(Room_CreateObject);
		STY_ADD_INTERNAL_CALL(Room_DestroyObject);

		STY_ADD_INTERNAL_CALL(Object_GetPosition);
		STY_ADD_INTERNAL_CALL(Object_SetPosition);
		STY_ADD_INTERNAL_CALL(Object_GetRotation);
		STY_ADD_INTERNAL_CALL(Object_SetRotation);
		STY_ADD_INTERNAL_CALL(Object_GetScale);
		STY_ADD_INTERNAL_CALL(Object_SetScale);

		STY_ADD_INTERNAL_CALL(Input_IsKeyPressed);
		STY_ADD_INTERNAL_CALL(Input_IsKeyHeld);
		STY_ADD_INTERNAL_CALL(Input_IsKeyDown);
		STY_ADD_INTERNAL_CALL(Input_IsKeyReleased);
		STY_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);
		STY_ADD_INTERNAL_CALL(Input_IsMouseButtonHeld);
		STY_ADD_INTERNAL_CALL(Input_IsMouseButtonDown);
		STY_ADD_INTERNAL_CALL(Input_IsMouseButtonReleased);
		STY_ADD_INTERNAL_CALL(Input_GetMousePosition);

		STY_ADD_INTERNAL_CALL(Log_LogMessage);
	}

	namespace InternalCalls {
		
		InstanceID Room_CreateObject(float x, float y, AssetHandle object)
		{
			InstanceID instance = Project::GetActiveRoom()->InstantiatePrefab(object);
			Project::GetActiveRoom()->GetObject(instance).Position = { x, y };

			return instance;
		}

		void Room_DestroyObject(InstanceID id)
		{
			Project::GetActiveRoom()->DestroyInstance(id);
		}

		void Object_GetPosition(InstanceID id, glm::vec2* outPosition)
		{
			*outPosition = Project::GetActiveRoom()->GetObject(id).Position;
		}

		void Object_SetPosition(InstanceID id, glm::vec2* inPosition)
		{
			Project::GetActiveRoom()->GetObject(id).Position = *inPosition;
		}

		void Object_GetScale(InstanceID id, glm::vec2* outScale)
		{
			*outScale = Project::GetActiveRoom()->GetObject(id).Scale;
		}

		void Object_SetScale(InstanceID id, glm::vec2* inScale)
		{
			Project::GetActiveRoom()->GetObject(id).Scale = *inScale;
		}

		void Object_GetRotation(InstanceID id, float* outRotation)
		{
			*outRotation = Project::GetActiveRoom()->GetObject(id).Rotation;
		}

		void Object_SetRotation(InstanceID id, float* inRotation)
		{
			Project::GetActiveRoom()->GetObject(id).Rotation = *inRotation;
		}

		Coral::Bool32 Input_IsKeyPressed(KeyCode keycode) { return Input::IsKeyPressed(keycode); }
		Coral::Bool32 Input_IsKeyHeld(KeyCode keycode) { return Input::IsKeyHeld(keycode); }
		Coral::Bool32 Input_IsKeyDown(KeyCode keycode) { return Input::IsKeyDown(keycode); }
		Coral::Bool32 Input_IsKeyReleased(KeyCode keycode) { return Input::IsKeyReleased(keycode); }
		Coral::Bool32 Input_IsMouseButtonPressed(MouseCode button) { return Input::IsMouseButtonPressed(button); }
		Coral::Bool32 Input_IsMouseButtonHeld(MouseCode button) { return Input::IsMouseButtonHeld(button); }
		Coral::Bool32 Input_IsMouseButtonDown(MouseCode button) { return Input::IsMouseButtonDown(button); }
		Coral::Bool32 Input_IsMouseButtonReleased(MouseCode button) { return Input::IsMouseButtonReleased(button); }

		void Input_GetMousePosition(glm::vec2* outPosition)
		{
			*outPosition = Input::GetMousePosition();
		}

		void Log_LogMessage(LogLevel level, Coral::String fmt)
		{
			std::string message = fmt;

			switch (level)
			{
			case LogLevel::Trace: STY_TRACE(message); break;
			case LogLevel::Info:  STY_INFO(message); break;
			case LogLevel::Warn:  STY_WARN(message); break;
			case LogLevel::Error: STY_ERROR(message); break;
			}

			Coral::String::Free(fmt);
		}

	}

}