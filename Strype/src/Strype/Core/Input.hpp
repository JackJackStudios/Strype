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

	enum class BindingType : uint16_t
	{
		Keyboard,
		MouseButton,
		GamepadButton,
		GamepadAxis,
	};

	struct InputBinding
	{
		union {
			KeyCode Keyboard;
			MouseCode MouseButton;
			ButtonCode GamepadButton;
			GamepadAxis GamepadAxis;
		} Value;

		BindingType Type;
	};

	constexpr float GAMEPAD_DEADZONE = 0.5;

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

		static bool IsGamepadAxisActive(GamepadAxis axis);

		// Binding API
		static bool IsBindingDown(InputBinding binding);
		static bool IsBindingPressed(InputBinding binding);
		static bool IsBindingHeld(InputBinding binding);
		static bool IsBindingReleased(InputBinding binding);

		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}