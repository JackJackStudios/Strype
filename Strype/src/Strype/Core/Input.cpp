#include "stypch.h"
#include "Strype/Core/Input.h"

#include "Strype/Core/Application.h"

#include "Strype/Events/KeyEvent.h"
#include "Strype/Events/MouseEvent.h"

#include <GLFW/glfw3.h>

namespace Strype {

	void Input::Update()
	{
		for (const auto& [key, data] : s_KeyStates)
		{
			if (data.State == InputState::Pressed)
				UpdateKeyState(key, InputState::Held);

			if (data.State == InputState::Released)
				UpdateKeyState(key, InputState::None);

			if (data.State == InputState::Held && !Input::IsKeyOn(key))
			{
				KeyReleasedEvent event(key);
				s_EventCallback(event);

				UpdateKeyState(key, InputState::Released);
			}

			if (data.State == InputState::None && Input::IsKeyOn(key))
			{
				KeyPressedEvent event(key);
				s_EventCallback(event);

				UpdateKeyState(key, InputState::Pressed);
			}

			if (data.State == InputState::Held)
			{
				KeyHeldEvent event(key);
				s_EventCallback(event);
			}
		}

		for (const auto& [button, data] : s_MouseStates)
		{
			if (data.State == InputState::Pressed)
				UpdateMouseState(button, InputState::Held);
		
			if (data.State == InputState::Released)
				UpdateMouseState(button, InputState::None);
		
			if (data.State == InputState::Held && !Input::IsMouseButtonOn(button))
			{
				MouseButtonReleasedEvent event(button);
				s_EventCallback(event);
		
				UpdateMouseState(button, InputState::Released);
			}
		
			if (data.State == InputState::None && Input::IsMouseButtonOn(button))
			{
				MouseButtonPressedEvent event(button);
				s_EventCallback(event);
		
				UpdateMouseState(button, InputState::Pressed);
			}
		
			if (data.State == InputState::Held)
			{
				MouseButtonHeldEvent event(button);
				s_EventCallback(event);
			}
		}
	}

	void Input::UpdateKeyState(KeyCode key, InputState newState)
	{
		auto& keyData = s_KeyStates[key];
		keyData.Key = key;
		keyData.OldState = keyData.State;
		keyData.State = newState;
	}

	void Input::UpdateMouseState(MouseCode key, InputState newState)
	{
		auto& keyData = s_MouseStates[key];
		keyData.Button = key;
		keyData.OldState = keyData.State;
		keyData.State = newState;
	}

	bool Input::IsKeyOn(const KeyCode key)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}

	bool Input::IsKeyPressed(const KeyCode key)
	{
		return s_KeyStates[key].State == InputState::Pressed;
	}

	bool Input::IsKeyHeld(const KeyCode key)
	{
		return s_KeyStates[key].State == InputState::Held;
	}

	bool Input::IsKeyReleased(const KeyCode key)
	{
		return s_KeyStates[key].State == InputState::Released;
	}

	bool Input::IsMouseButtonOn(const MouseCode button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		return s_MouseStates[button].State == InputState::Released;
	}

	bool Input::IsMouseButtonHeld(MouseCode button)
	{
		return s_MouseStates[button].State == InputState::Released;
	}

	bool Input::IsMouseButtonReleased(MouseCode button)
	{
		return s_MouseStates[button].State == InputState::Released;
	}

	glm::vec2 Input::GetMousePosition()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
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