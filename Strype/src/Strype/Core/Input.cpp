#include "stypch.hpp"
#include "Input.hpp"

#include "Strype/Core/Application.hpp"
#include "Strype/Core/Event.hpp"

#include <GLFW/glfw3.h>

namespace Strype {

	void Input::Init()
	{
		auto& window = Application::Get().GetWindow();

		// NOTE: These callbacks enter keys into the map
		//       so they will be tracked by Input::Update().
		window->SetKeyCallback([](AGI::Window* window, int key, int scancode, int action, int mods)
		{
			if (action == GLFW_PRESS)
				s_KeyStates[(KeyCode)key] = InputState::None;
		});

		window->SetMouseButtonCallback([](AGI::Window* window, int button, int action, int mods)
		{
			if (action == GLFW_PRESS)
				s_MouseStates[(MouseCode)button] = InputState::None;
		});
	}

	void Input::Update()
	{
		for (const auto& [key, data] : s_KeyStates)
		{
			if (data == InputState::Pressed)
				s_KeyStates[key] = InputState::Held;

			if (data == InputState::Released)
				s_KeyStates[key] = InputState::None;

			if (data == InputState::Held && !Input::IsKeyDown(key))
			{
				KeyReleasedEvent event(key);
				Application::Get().OnEvent(event);

				s_KeyStates[key] = InputState::Released;
			}

			if (data == InputState::None && Input::IsKeyDown(key))
			{
				KeyPressedEvent event(key);
				Application::Get().OnEvent(event);

				s_KeyStates[key] = InputState::Pressed;
			}

			if (data == InputState::Held)
			{
				KeyHeldEvent event(key);
				Application::Get().OnEvent(event);
			}
		}

		for (const auto& [button, data] : s_MouseStates)
		{
			if (data == InputState::Pressed)
				s_MouseStates[button] = InputState::Held;
		
			if (data == InputState::Released)
				s_MouseStates[button] = InputState::None;
		
			if (data == InputState::Held && !Input::IsMouseButtonDown(button))
			{
				MouseButtonReleasedEvent event(button);
				Application::Get().OnEvent(event);
		
				s_MouseStates[button] = InputState::Released;
			}
		
			if (data == InputState::None && Input::IsMouseButtonDown(button))
			{
				MouseButtonPressedEvent event(button);
				Application::Get().OnEvent(event);
		
				s_MouseStates[button] = InputState::Pressed;
			}
		
			if (data == InputState::Held)
			{
				MouseButtonHeldEvent event(button);
				Application::Get().OnEvent(event);
			}
		}
	}

	bool Input::IsKeyDown(const KeyCode key)
	{
		GLFWwindow* window = Application::Get().GetWindow()->GetGlfwWindow();
		int state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}

	bool Input::IsKeyPressed(const KeyCode key)
	{
		return s_KeyStates[key] == InputState::Pressed;
	}

	bool Input::IsKeyHeld(const KeyCode key)
	{
		return s_KeyStates[key] == InputState::Held;
	}

	bool Input::IsKeyReleased(const KeyCode key)
	{
		return s_KeyStates[key] == InputState::Released;
	}

	bool Input::IsMouseButtonDown(const MouseCode button)
	{
		GLFWwindow* window = Application::Get().GetWindow()->GetGlfwWindow();
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		return s_MouseStates[button] == InputState::Released;
	}

	bool Input::IsMouseButtonHeld(MouseCode button)
	{
		return s_MouseStates[button] == InputState::Released;
	}

	bool Input::IsMouseButtonReleased(MouseCode button)
	{
		return s_MouseStates[button] == InputState::Released;
	}

	glm::vec2 Input::GetMousePosition()
	{
		GLFWwindow* window = Application::Get().GetWindow()->GetGlfwWindow();
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