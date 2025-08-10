#include "stypch.hpp"
#include "Input.hpp"

#include "Strype/Core/Application.hpp"
#include "Strype/Core/Event.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Strype {

	struct InputData
	{
		std::map<KeyCode, InputState> Keys;
		std::map<MouseCode, InputState> MouseButtons;
		std::map<ButtonCode, InputState> GamepadButtons;

		bool ConnectedGamepad = false;
		int GamepadID = 0; // jid

		GLFWgamepadstate GamepadState;
	};

	static thread_local InputData s_InputState;

	void OnConnectGamepad(int jid, int event)
	{
		if (event == GLFW_CONNECTED && glfwJoystickIsGamepad(jid))
		{
			s_InputState.ConnectedGamepad = true;
			s_InputState.GamepadID = jid;

			STY_CORE_INFO("Gamepad connected: \"{}\" (GUID = {})", glfwGetJoystickName(jid), glfwGetJoystickGUID(jid));
		}
		else if (event == GLFW_DISCONNECTED)
		{
			s_InputState.ConnectedGamepad = false;
			s_InputState.GamepadID = 0;
		}
	}

	void Input::Init()
	{
		auto window = Application::Get().GetWindow();

		// NOTE: These callbacks enter keys into the map
		//       so they will be tracked by Input::Update().
		window->SetKeyCallback([](AGI::Window* window, int key, int scancode, int action, int mods)
		{
			if (action == GLFW_PRESS || action == GLFW_RELEASE)
				s_InputState.Keys[(KeyCode)key] = InputState::None;
		});

		window->SetMouseButtonCallback([](AGI::Window* window, int button, int action, int mods)
		{
			if (action == GLFW_PRESS || action == GLFW_RELEASE)
				s_InputState.MouseButtons[(MouseCode)button] = InputState::None;
		});

		glfwSetJoystickCallback(OnConnectGamepad);
		for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++)
		{
			if (glfwJoystickPresent(jid))
				OnConnectGamepad(jid, GLFW_CONNECTED);
		}
	}

	void Input::Update()
	{
		for (auto& [key, state] : s_InputState.Keys)
		{
			if (state == InputState::Pressed)
				state = InputState::Held;

			if (state == InputState::Released)
				state = InputState::None;

			if (state == InputState::Held && !Input::IsKeyDown(key))
			{
				KeyReleasedEvent event(key);
				Application::Get().OnEvent(event);

				state = InputState::Released;
			}

			if (state == InputState::None && Input::IsKeyDown(key))
			{
				KeyPressedEvent event(key);
				Application::Get().OnEvent(event);

				state = InputState::Pressed;
			}
		}

		for (auto& [button, state] : s_InputState.MouseButtons)
		{
			if (state == InputState::Pressed)
				state = InputState::Held;

			if (state == InputState::Released)
				state = InputState::None;

			if (state == InputState::Held && !Input::IsMouseButtonDown(button))
			{
				MouseButtonReleasedEvent event(button);
				Application::Get().OnEvent(event);

				state = InputState::Released;
			}

			if (state == InputState::None && Input::IsMouseButtonDown(button))
			{
				MouseButtonPressedEvent event(button);
				Application::Get().OnEvent(event);

				state = InputState::Pressed;
			}
		}

		if (s_InputState.ConnectedGamepad)
		{
			if (glfwGetGamepadState(s_InputState.GamepadID, &s_InputState.GamepadState))
			{
				// NOTE: 15 is the total size of GLFWgamepadstate::buttons
				for (int i = 0; i < 15; i++)
				{
					ButtonCode button = (ButtonCode)i;
					bool buttonDown = s_InputState.GamepadState.buttons[i] == GLFW_PRESS;

					auto it = s_InputState.GamepadButtons.find(button);
					InputState state = (it == s_InputState.GamepadButtons.end()) ? InputState::None : it->second;

					if (state == InputState::None && !buttonDown)
					{
						if (it != s_InputState.GamepadButtons.end())
							s_InputState.GamepadButtons.erase(button);

						continue;
					}

					if (state == InputState::Pressed)
						s_InputState.GamepadButtons[button] = InputState::Held;

					if (state == InputState::Released)
						s_InputState.GamepadButtons[button] = InputState::None;

					if (state == InputState::Held && !buttonDown)
						s_InputState.GamepadButtons[button] = InputState::Released;

					if (state == InputState::None && buttonDown)
						s_InputState.GamepadButtons[button] = InputState::Pressed;
				}
			}
		}
	}

	bool Input::IsKeyDown(KeyCode key)
	{
		auto window = Application::Get().GetWindow();
		return glfwGetKey(window->GetGlfwWindow(), (int32_t)key) == GLFW_PRESS;
	}

	bool Input::IsMouseButtonDown(MouseCode button)
	{
		auto window = Application::Get().GetWindow();
		return glfwGetMouseButton(window->GetGlfwWindow(), (int32_t)button) == GLFW_PRESS;
	}

	bool Input::IsGamepadButtonDown(ButtonCode button)
	{
		if (s_InputState.GamepadButtons.find(button) != s_InputState.GamepadButtons.end())
			if (s_InputState.GamepadButtons[button] != InputState::Released) // Released is the frame after the button is let go
				return true;

		return false;
	}

	bool Input::IsKeyPressed(KeyCode key)
	{
		if (s_InputState.Keys.find(key) == s_InputState.Keys.end()) return false;
		return s_InputState.Keys[key] == InputState::Pressed;
	}

	bool Input::IsKeyHeld(KeyCode key)
	{
		if (s_InputState.Keys.find(key) == s_InputState.Keys.end()) return false;
		return s_InputState.Keys[key] == InputState::Held;
	}

	bool Input::IsKeyReleased(KeyCode key)
	{
		if (s_InputState.Keys.find(key) == s_InputState.Keys.end()) return false;
		return s_InputState.Keys[key] == InputState::Released;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		if (s_InputState.MouseButtons.find(button) == s_InputState.MouseButtons.end()) return false;
		return s_InputState.MouseButtons[button] == InputState::Pressed;
	}

	bool Input::IsMouseButtonHeld(MouseCode button)
	{
		if (s_InputState.MouseButtons.find(button) == s_InputState.MouseButtons.end()) return false;
		return s_InputState.MouseButtons[button] == InputState::Held;
	}

	bool Input::IsMouseButtonReleased(MouseCode button)
	{
		if (s_InputState.MouseButtons.find(button) == s_InputState.MouseButtons.end()) return false;
		return s_InputState.MouseButtons[button] == InputState::Released;
	}

	bool Input::IsGamepadButtonPressed(ButtonCode button)
	{
		if (!s_InputState.ConnectedGamepad) return false;
		if (s_InputState.GamepadButtons.find(button) == s_InputState.GamepadButtons.end()) return false;
		return s_InputState.GamepadButtons[button] == InputState::Pressed;
	}

	bool Input::IsGamepadButtonHeld(ButtonCode button)
	{
		if (!s_InputState.ConnectedGamepad) return false;
		if (s_InputState.GamepadButtons.find(button) == s_InputState.GamepadButtons.end()) return false;
		return s_InputState.GamepadButtons[button] == InputState::Held;
	}

	bool Input::IsGamepadButtonReleased(ButtonCode button)
	{
		if (!s_InputState.ConnectedGamepad) return false;
		if (s_InputState.GamepadButtons.find(button) == s_InputState.GamepadButtons.end()) return false;
		return s_InputState.GamepadButtons[button] == InputState::Released;
	}

	bool Input::IsGamepadAxisActive(GamepadAxis axis)
	{
		if (!s_InputState.ConnectedGamepad) return false;
		int axisIndex = -1;
		bool positiveDir = false;

		switch (axis)
		{
		case GamepadAxis::LeftLeft:  axisIndex = GLFW_GAMEPAD_AXIS_LEFT_X;  positiveDir = false; break;
		case GamepadAxis::LeftRight: axisIndex = GLFW_GAMEPAD_AXIS_LEFT_X;  positiveDir = true;  break;
		case GamepadAxis::LeftUp:    axisIndex = GLFW_GAMEPAD_AXIS_LEFT_Y;  positiveDir = false; break;
		case GamepadAxis::LeftDown:  axisIndex = GLFW_GAMEPAD_AXIS_LEFT_Y;  positiveDir = true;  break;

		case GamepadAxis::RightLeft:  axisIndex = GLFW_GAMEPAD_AXIS_RIGHT_X; positiveDir = false; break;
		case GamepadAxis::RightRight: axisIndex = GLFW_GAMEPAD_AXIS_RIGHT_X; positiveDir = true;  break;
		case GamepadAxis::RightUp:    axisIndex = GLFW_GAMEPAD_AXIS_RIGHT_Y; positiveDir = false; break;
		case GamepadAxis::RightDown:  axisIndex = GLFW_GAMEPAD_AXIS_RIGHT_Y; positiveDir = true;  break;
		}

		float value = s_InputState.GamepadState.axes[axisIndex];

		// NEW: clamp deadzone both directions
		if (fabs(value) < GAMEPAD_DEADZONE)
			return false;

		return positiveDir ? (value > 0) : (value < 0);
	}

	bool Input::IsBindingDown(InputBinding binding) 
	{
		switch (binding.Type)
		{
		case BindingType::Keyboard:      return IsKeyDown(binding.Value.Keyboard);
		case BindingType::MouseButton:   return IsMouseButtonDown(binding.Value.MouseButton);
		case BindingType::GamepadButton: return IsGamepadButtonDown(binding.Value.GamepadButton);
		case BindingType::GamepadAxis:   return IsGamepadAxisActive(binding.Value.GamepadAxis);
		}

		STY_CORE_VERIFY(false, "Unsupported InputBinding");
		return false;
	}

	bool Input::IsBindingPressed(InputBinding binding) 
	{
		switch (binding.Type)
		{
		case BindingType::Keyboard:      return IsKeyPressed(binding.Value.Keyboard);
		case BindingType::MouseButton:   return IsMouseButtonPressed(binding.Value.MouseButton);
		case BindingType::GamepadButton: return IsGamepadButtonPressed(binding.Value.GamepadButton);
		case BindingType::GamepadAxis:   return IsGamepadAxisActive(binding.Value.GamepadAxis);
		}

		STY_CORE_VERIFY(false, "Unsupported InputBinding");
		return false;
	}

	bool Input::IsBindingHeld(InputBinding binding) 
	{
		switch (binding.Type)
		{
		case BindingType::Keyboard:      return IsKeyHeld(binding.Value.Keyboard);
		case BindingType::MouseButton:   return IsMouseButtonHeld(binding.Value.MouseButton);
		case BindingType::GamepadButton: return IsGamepadButtonHeld(binding.Value.GamepadButton);
		case BindingType::GamepadAxis:   return IsGamepadAxisActive(binding.Value.GamepadAxis);
		}

		STY_CORE_VERIFY(false, "Unsupported InputBinding");
		return false;
	}

	bool Input::IsBindingReleased(InputBinding binding) 
	{
		switch (binding.Type)
		{
		case BindingType::Keyboard:      return IsKeyReleased(binding.Value.Keyboard);
		case BindingType::MouseButton:   return IsMouseButtonReleased(binding.Value.MouseButton);
		case BindingType::GamepadButton: return IsGamepadButtonReleased(binding.Value.GamepadButton);
		case BindingType::GamepadAxis:   return !IsGamepadAxisActive(binding.Value.GamepadAxis);
		}

		STY_CORE_VERIFY(false, "Unsupported InputBinding");
		return false;
	}

	glm::vec2 Input::GetMousePosition()
	{
		return Application::Get().GetWindow()->GetCursorPos();
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

}