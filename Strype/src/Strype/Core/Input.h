#pragma once

#include "Strype/Core/KeyCodes.h"
#include "Strype/Core/MouseCodes.h"
#include "Strype/Events/Event.h"

#include <glm/glm.hpp>

namespace Strype {

	enum class InputState
	{
		None = -1,
		Pressed,
		Held,
		Released,
	};

	struct KeyData
	{
		KeyCode Key;
		InputState State = InputState::None;
		InputState OldState = InputState::None;
	};

	struct MouseData
	{
		MouseCode Button;
		InputState State = InputState::None;
		InputState OldState = InputState::None;
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

		static bool IsMouseButtonOn(MouseCode button); //GLFW

		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonHeld(MouseCode button);
		static bool IsMouseButtonReleased(MouseCode button);

		static glm::vec2 GetMousePosition(); //GLFW
		static float GetMouseX(); //GLFW
		static float GetMouseY(); //GLFW

		static void UpdateKeyState(KeyCode key, InputState newState);
		static void UpdateMouseState(MouseCode key, InputState newState);

		static inline void SetEventCallback(const EventCallbackFn& callback) { s_EventCallback = callback; };
	private:
		inline static std::unordered_map<KeyCode, KeyData> s_KeyStates;
		inline static std::unordered_map<MouseCode, MouseData> s_MouseStates;
		inline static EventCallbackFn s_EventCallback;
	};
}