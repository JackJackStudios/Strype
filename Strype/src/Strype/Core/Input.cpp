#include "stypch.h"
#include "Strype/Core/Input.h"

#include "Strype/Events/KeyEvent.h"

namespace Strype {

	void Input::Update()
	{
		for (const auto& [key, data] : s_KeyStates)
		{
			if (data.State == KeyState::Pressed)
				UpdateKeyState(key, KeyState::Held);

			if (data.State == KeyState::Released)
				UpdateKeyState(key, KeyState::None);

			if (data.State == KeyState::Held && !Input::IsKeyOn(key))
			{
				KeyReleasedEvent event(key);
				s_EventCallback(event);

				UpdateKeyState(key, KeyState::Released);
			}

			if (data.State == KeyState::None && Input::IsKeyOn(key))
			{
				KeyPressedEvent event(key);
				s_EventCallback(event);

				UpdateKeyState(key, KeyState::Pressed);
			}

			if (data.State == KeyState::Held)
			{
				KeyHeldEvent event(key);
				s_EventCallback(event);
			}
		}
	}

	void Input::UpdateKeyState(KeyCode key, KeyState newState)
	{
		auto& keyData = s_KeyStates[key];
		keyData.Key = key;
		keyData.OldState = keyData.State;
		keyData.State = newState;
	}

	bool Input::IsKeyPressed(const KeyCode key)
	{
		return s_KeyStates[key].State == KeyState::Pressed;
	}

	bool Input::IsKeyHeld(const KeyCode key)
	{
		return s_KeyStates[key].State == KeyState::Held;
	}

	bool Input::IsKeyReleased(const KeyCode key)
	{
		return s_KeyStates[key].State == KeyState::Released;
	}

}