#include "stypch.hpp"

#include "ScriptGlue.hpp"
#include "ScriptEngine.hpp"

#include "Strype/Room/Room.hpp"
#include "Strype/Project/Project.hpp"

#include "Strype/Core/AudioFile.hpp"
#include "Strype/Core/Application.hpp"
#include "Strype/Core/Input.hpp"

#define STY_ADD_INTERNAL_CALL(icall) s_CoreAssembly->AddInternalCall("Strype.InternalCalls", #icall, (void*)InternalCalls::icall)

namespace Strype {

	void ScriptEngine::RegisterInternalCalls()
	{
		STY_ADD_INTERNAL_CALL(Room_CreateObject);
		STY_ADD_INTERNAL_CALL(Room_DestroyObject);
		STY_ADD_INTERNAL_CALL(Room_GetManager);
		STY_ADD_INTERNAL_CALL(Room_TransitionRoom);
		STY_ADD_INTERNAL_CALL(Camera_Move);
		STY_ADD_INTERNAL_CALL(Camera_Zoom);

		STY_ADD_INTERNAL_CALL(Audio_PlaySound);
		STY_ADD_INTERNAL_CALL(Audio_PlaySoundOn);

		STY_ADD_INTERNAL_CALL(Object_GetPosition);
		STY_ADD_INTERNAL_CALL(Object_SetPosition);
		STY_ADD_INTERNAL_CALL(Object_GetRotation);
		STY_ADD_INTERNAL_CALL(Object_SetRotation);
		STY_ADD_INTERNAL_CALL(Object_GetScale);
		STY_ADD_INTERNAL_CALL(Object_SetScale);

		STY_ADD_INTERNAL_CALL(Input_IsVerbPressed);
		STY_ADD_INTERNAL_CALL(Input_IsVerbHeld);
		STY_ADD_INTERNAL_CALL(Input_IsVerbDown);
		STY_ADD_INTERNAL_CALL(Input_IsVerbReleased);

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
			RoomInstance instance;
			instance.Position = { x, y };
			instance.ObjectHandle = object;
			return Project::GetActiveRoom()->CreateInstance(instance);
		}

		void Room_DestroyObject(uint32_t id)
		{
			Project::GetActiveRoom()->DestroyInstance(id);
		}

		void* Room_GetManager(Coral::String name)
		{
			return Project::GetActiveRoom()->GetManager((std::string)name)->GetGCHandle();
		}

		void Room_TransitionRoom(Coral::String name)
		{
			std::string cxxname = name;
			AssetHandle handle = Project::GetAssetManager()->GetHandle(cxxname);

			if (handle == 0)
			{
				STY_LOG_WARN("Script", "Invalid path for Room: \"{}\" ", cxxname);
				return;
			}

			Application::Get().DispatchEvent<RoomTransitionEvent>(handle);
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

		void Audio_PlaySound(Coral::String path)
		{
			std::string cxxpath = path;

			Ref<AudioFile> asset = Project::GetAsset<AudioFile>(Project::GetAssetManager()->GetHandle(cxxpath));
			if (asset == nullptr)
			{
				STY_LOG_WARN("Script", "Invalid path for AudioFile: \"{}\" ", cxxpath);
				return;
			}

			asset->Play();
		}

		void Audio_PlaySoundOn(Coral::String path, glm::vec2* inPosition)
		{
			std::string cxxpath = path;
			Ref<AssetManager> manager = Project::GetAssetManager();

			if (!manager->IsAssetLoaded(cxxpath))
			{
				STY_LOG_WARN("Script", "Invalid path for AudioFile: \"{}\" ", cxxpath);
				return;
			}


			Project::GetAsset<AudioFile>(manager->GetHandle(cxxpath))->PlayOn(*inPosition);
		}

		void Object_GetPosition(uint32_t id, glm::vec2* outPosition)
		{
			*outPosition = Project::GetActiveRoom()->GetInstance(id).Position;
		}

		void Object_SetPosition(uint32_t id, glm::vec2* inPosition)
		{
			Project::GetActiveRoom()->GetInstance(id).Position = *inPosition;
		}

		void Object_GetScale(uint32_t id, glm::vec2* outScale)
		{
			*outScale = Project::GetActiveRoom()->GetInstance(id).Scale;
		}

		void Object_SetScale(uint32_t id, glm::vec2* inScale)
		{
			Project::GetActiveRoom()->GetInstance(id).Scale = *inScale;
		}

		void Object_GetRotation(uint32_t id, float* outRotation)
		{
			*outRotation = Project::GetActiveRoom()->GetInstance(id).Rotation;
		}

		void Object_SetRotation(uint32_t id, float* inRotation)
		{
			Project::GetActiveRoom()->GetInstance(id).Rotation = *inRotation;
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
			case LogLevel::Trace: STY_LOG_TRACE("Runtime", message); break;
			case LogLevel::Info:  STY_LOG_INFO("Runtime", message); break;
			case LogLevel::Warn:  STY_LOG_WARN("Runtime", message); break;
			case LogLevel::Error: STY_LOG_ERROR("Runtime", message); break;
			}

			Coral::String::Free(fmt);
		}

	}

}