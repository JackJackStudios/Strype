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

		static bool IsKeyDown(KeyCode key);

		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyHeld(KeyCode key);
		static bool IsKeyReleased(KeyCode key);

		static bool IsMouseButtonDown(MouseCode button);

		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonHeld(MouseCode button);
		static bool IsMouseButtonReleased(MouseCode button);

		static bool IsGamepadButtonDown(ButtonCode button);

		static bool IsGamepadButtonPressed(ButtonCode button);
		static bool IsGamepadButtonHeld(ButtonCode button);
		static bool IsGamepadButtonReleased(ButtonCode button);

		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}