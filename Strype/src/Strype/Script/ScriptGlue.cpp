#include "stypch.hpp"

#include "ScriptGlue.hpp"
#include "ScriptEngine.hpp"

#include "Strype/Room/Room.hpp"
#include "Strype/Room/RoomInstance.hpp"

#include "Strype/Core/Input.hpp"
#include "Strype/Project/Project.hpp"

#define STY_ADD_INTERNAL_CALL(icall) s_CoreAssembly->AddInternalCall("Strype.InternalCalls", #icall, (void*)InternalCalls::icall)

namespace Strype {

	void ScriptEngine::RegisterInternalCalls()
	{
		STY_ADD_INTERNAL_CALL(Room_CreateObject);
		STY_ADD_INTERNAL_CALL(Room_DestroyObject);
		STY_ADD_INTERNAL_CALL(Camera_Move);
		STY_ADD_INTERNAL_CALL(Camera_Zoom);

		STY_ADD_INTERNAL_CALL(Object_GetPosition);
		STY_ADD_INTERNAL_CALL(Object_SetPosition);
		STY_ADD_INTERNAL_CALL(Object_GetRotation);
		STY_ADD_INTERNAL_CALL(Object_SetRotation);
		STY_ADD_INTERNAL_CALL(Object_GetScale);
		STY_ADD_INTERNAL_CALL(Object_SetScale);

		// STY_ADD_INTERNAL_CALL(Input_IsVerbPressed);
		// STY_ADD_INTERNAL_CALL(Input_IsVerbHeld);
		// STY_ADD_INTERNAL_CALL(Input_IsVerbDown);
		// STY_ADD_INTERNAL_CALL(Input_IsVerbReleased);

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
		
		uint32_t Room_CreateObject(float x, float y, uint64_t object)
		{
			auto instance = Project::GetActiveRoom()->CreateInstance(object);
			Project::GetActiveRoom()->GetObject(instance).Position = { x, y };

			return instance;
		}

		void Room_DestroyObject(uint32_t id)
		{
			Project::GetActiveRoom()->DestroyInstance(id);
		}

		void Camera_Move(glm::vec2* inPosition)
		{
			auto& camera = Project::GetActiveRoom()->GetCamera();
			camera.Position = *inPosition;
		}

		void Camera_Zoom(float* inZoomLevel)
		{
			auto& camera = Project::GetActiveRoom()->GetCamera();
			camera.SetZoomLevel(*inZoomLevel);
		}

		void Object_GetPosition(uint32_t id, glm::vec2* outPosition)
		{
			*outPosition = Project::GetActiveRoom()->GetObject(id).Position;
		}

		void Object_SetPosition(uint32_t id, glm::vec2* inPosition)
		{
			Project::GetActiveRoom()->GetObject(id).Position = *inPosition;
		}

		void Object_GetScale(uint32_t id, glm::vec2* outScale)
		{
			*outScale = Project::GetActiveRoom()->GetObject(id).Scale;
		}

		void Object_SetScale(uint32_t id, glm::vec2* inScale)
		{
			Project::GetActiveRoom()->GetObject(id).Scale = *inScale;
		}

		void Object_GetRotation(uint32_t id, float* outRotation)
		{
			*outRotation = Project::GetActiveRoom()->GetObject(id).Rotation;
		}

		void Object_SetRotation(uint32_t id, float* inRotation)
		{
			Project::GetActiveRoom()->GetObject(id).Rotation = *inRotation;
		}

		Coral::Bool32 Input_IsVerbPressed(Coral::String verb)
		{
			std::string message = verb;
			if (!Project::GetActive()->VerbExists(message)) return false;

			for (const auto& binding : Project::GetActive()->GetBindings(message))
			{
				if (Input::IsBindingPressed(binding))
					return true;
			}

			return false;
		}

		Coral::Bool32 Input_IsVerbHeld(Coral::String verb)
		{
			std::string message = verb;
			if (!Project::GetActive()->VerbExists(message)) return false;

			for (const auto& binding : Project::GetActive()->GetBindings(message))
			{
				if (Input::IsBindingHeld(binding))
					return true;
			}

			return false;
		}

		Coral::Bool32 Input_IsVerbDown(Coral::String verb)
		{
			std::string message = verb;
			if (!Project::GetActive()->VerbExists(message)) return false;

			for (const auto& binding : Project::GetActive()->GetBindings(message))
			{
				if (Input::IsBindingDown(binding))
					return true;
			}

			return false;
		}

		Coral::Bool32 Input_IsVerbReleased(Coral::String verb)
		{
			std::string message = verb;
			if (!Project::GetActive()->VerbExists(message)) return false;

			for (const auto& binding : Project::GetActive()->GetBindings(message))
			{
				if (Input::IsBindingReleased(binding))
					return true;
			}

			return false;
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