#pragma once

#include <Coral/Assembly.hpp>
#include <Coral/String.hpp>
#include <Coral/Array.hpp>

#include <glm/glm.hpp>

#include "TypeInfo.h"

#include "Strype/Core/Input.h"

namespace Strype {

	class ScriptGlue
	{
	public:
		static void RegisterGlue(Coral::ManagedAssembly& coreAssembly);

	private:
		static void RegisterComponentTypes(Coral::ManagedAssembly& coreAssembly);
		static void RegisterInternalCalls(Coral::ManagedAssembly& coreAssembly);
	};

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
			Critical = BIT(5)
		};
		
		void Object_CreateComponent(uint32_t id, Coral::ReflectionType componentType);
		bool Object_HasComponent(uint32_t id, Coral::ReflectionType componentType);
		bool Object_RemoveComponent(uint32_t id, Coral::ReflectionType componentType);

		bool Room_ObjectExists(uint32_t id);
		void Room_DestroyObject(uint32_t id);

		void Transform_GetPosition(uint32_t id, glm::vec2* outTranslation);
		void Transform_SetPosition(uint32_t id, glm::vec2* inTranslation);
		void Transform_GetRotation(uint32_t id, float* outRotation);
		void Transform_SetRotation(uint32_t id, float* inRotation);
		void Transform_GetScale(uint32_t id, glm::vec2* outScale);
		void Transform_SetScale(uint32_t id, glm::vec2* inScale);
		
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