#pragma once

#include "Strype/Core/KeyCodes.h"
#include "Strype/Core/MouseCodes.h"
#include "Strype/Events/Event.h"

#include <glm/glm.hpp>

namespace Strype {

	struct KeyData
	{
		KeyCode Key;
		KeyState State = KeyState::None;
		KeyState OldState = KeyState::None;
	};

	class Input
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		static void Update();

		static bool IsKeyOn(KeyCode key); //GLFW

		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyHeld(KeyCode key);
		static bool IsKeyReleased(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button); //GLFW
		static glm::vec2 GetMousePosition(); //GLFW
		static float GetMouseX(); //GLFW
		static float GetMouseY(); //GLFW

		static inline void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; };
	private:
		static std::unordered_map<KeyCode, KeyState> m_KeyStates;
		static EventCallbackFn m_EventCallback;
	};
}