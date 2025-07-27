#pragma once

#include "Strype/Core/InputCodes.hpp"

#include <glm/glm.hpp>

namespace Strype {

	enum class InputState
	{
		None = -1,
		Pressed,
		Held,
		Released,
	};

	class Input
	{
	public:
		static void Init();
		static void Update();

		static bool IsKeyDown(KeyCode key); //GLFW

		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyHeld(KeyCode key);
		static bool IsKeyReleased(KeyCode key);

		static bool IsMouseButtonDown(MouseCode button); //GLFW

		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonHeld(MouseCode button);
		static bool IsMouseButtonReleased(MouseCode button);

		static glm::vec2 GetMousePosition(); //GLFW
		static float GetMouseX(); //GLFW
		static float GetMouseY(); //GLFW
	private:
		inline static thread_local std::unordered_map<KeyCode, InputState> s_KeyStates;
		inline static thread_local std::unordered_map<MouseCode, InputState> s_MouseStates;
	};
}