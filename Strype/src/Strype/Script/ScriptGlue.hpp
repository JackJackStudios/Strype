#pragma once

#include <Coral/Assembly.hpp>
#include <Coral/String.hpp>
#include <Coral/Array.hpp>

#include <glm/glm.hpp>

#include "Strype/Utils/TypeInfo.hpp"

#include "Strype/Core/Input.hpp"
#include "Strype/Room/Room.hpp"

namespace Strype {

	namespace InternalCalls {

		template<std::default_initializable T>
		struct Param
		{
			std::byte Data[sizeof(T)];

			operator T() const
			{
				T result;
				std::memcpy(&result, Data, sizeof(T));
				return result;
			}
		};

		template<typename T>
		struct OutParam
		{
			std::byte* Ptr = nullptr;

			T* operator->() noexcept { return reinterpret_cast<T*>(Ptr); }
			const T* operator->() const noexcept { return reinterpret_cast<const T*>(Ptr); }

			T& operator*() { return *reinterpret_cast<T*>(Ptr); }
			const T& operator*() const { return *reinterpret_cast<const T*>(Ptr); }
		};

		enum class LogLevel : int32_t
		{
			Trace = BIT(0),
			Info = BIT(2),
			Warn = BIT(3),
			Error = BIT(4),
		};

		uint32_t Room_CreateObject(float x, float y, uint64_t object);
		void Room_DestroyObject(uint32_t id);
		void Room_DispatchEvent(Coral::String name);
		void* Room_GetManager(Coral::String name);
		void Room_TransitionRoom(Coral::String name);
		void Camera_Move(glm::vec2* inPosition);
		void Camera_Zoom(float* inZoomLevel);

		void Audio_PlaySound(Coral::String path);
		void Audio_PlaySoundOn(Coral::String path, glm::vec2* inPosition);

		void Object_GetPosition(uint32_t id, glm::vec2* outPosition);
		void Object_SetPosition(uint32_t id, glm::vec2* inPosition);
		void Object_GetScale(uint32_t id, glm::vec2* outScale);
		void Object_SetScale(uint32_t id, glm::vec2* inScale);
		void Object_GetRotation(uint32_t id, float* outRotation);
		void Object_SetRotation(uint32_t id, float* inRotation);

		Coral::Bool32 Input_IsVerbPressed(Coral::String verb);
		Coral::Bool32 Input_IsVerbHeld(Coral::String verb);
		Coral::Bool32 Input_IsVerbDown(Coral::String verb);
		Coral::Bool32 Input_IsVerbReleased(Coral::String verb);

		Coral::Bool32 Input_IsKeyPressed(KeyCode keycode);
		Coral::Bool32 Input_IsKeyHeld(KeyCode keycode);
		Coral::Bool32 Input_IsKeyDown(KeyCode keycode);
		Coral::Bool32 Input_IsKeyReleased(KeyCode keycode);
		Coral::Bool32 Input_IsMouseButtonPressed(MouseCode button);
		Coral::Bool32 Input_IsMouseButtonHeld(MouseCode button);
		Coral::Bool32 Input_IsMouseButtonDown(MouseCode button);
		Coral::Bool32 Input_IsMouseButtonReleased(MouseCode button);
		void Input_GetMousePosition(glm::vec2* outPosition);

		void Log_LogMessage(LogLevel level, Coral::String fmt);

	}

}