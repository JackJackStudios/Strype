#include "stypch.h"
#include "Strype/Core/Input.h"

#include "Strype/Events/KeyEvent.h"

namespace Strype {
	 
	std::unordered_map<KeyCode, KeyState> Input::m_KeyStates;
	Input::EventCallbackFn Input::m_EventCallback;

	void Input::Update()
	{
		for (const auto& [key, state] : m_KeyStates)
		{
			if (state == KeyState::Pressed)
				m_KeyStates[key] = KeyState::Held;

			if (state == KeyState::Released)
				m_KeyStates[key] = KeyState::None;

			if (state == KeyState::Held && !Input::IsKeyOn(key))
			{
				KeyReleasedEvent event(key);
				m_EventCallback(event);

				m_KeyStates[key] = KeyState::Released;
			}

			if (state == KeyState::None && Input::IsKeyOn(key))
			{
				KeyPressedEvent event(key);
				m_EventCallback(event);

				m_KeyStates[key] = KeyState::Pressed;
			}

			if (state == KeyState::Held)
			{
				KeyHeldEvent event(key);
				m_EventCallback(event);
			}
		}
	}

	bool Input::IsKeyPressed(const KeyCode key)
	{
		return m_KeyStates[key] == KeyState::Pressed;
	}

	bool Input::IsKeyHeld(const KeyCode key)
	{
		return m_KeyStates[key] == KeyState::Held;
	}

	bool Input::IsKeyReleased(const KeyCode key)
	{
		return m_KeyStates[key] == KeyState::Released;
	}

}