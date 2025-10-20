#pragma once

#include "Strype/Core/InputCodes.hpp"

#include <glm/glm.hpp>
#include <magic_enum/magic_enum.hpp>

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
		None = 0,
		Keyboard,
		MouseButton,
		GamepadButton,
		GamepadAxis,
	};

	struct InputBinding
	{
		BindingType Type = BindingType::None;
		int Code; // All codes for types above fit in a int

		bool operator==(const InputBinding& other) const
		{
			return Code == other.Code && Type == other.Type;
		}
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

};

template <>
struct fmt::formatter<Strype::InputBinding>
{
	constexpr auto parse(format_parse_context& ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const Strype::InputBinding& binding, FormatContext& ctx) const
	{
		std::string str = magic_enum::enum_name(binding.Type).data();
		str.append("::");

		switch (binding.Type)
		{
		case Strype::BindingType::Keyboard: str.append(magic_enum::enum_name((Strype::KeyCode)binding.Code)); break;
		case Strype::BindingType::MouseButton: str.append(magic_enum::enum_name((Strype::MouseCode)binding.Code)); break;
		case Strype::BindingType::GamepadButton: str.append(magic_enum::enum_name((Strype::ButtonCode)binding.Code)); break;
		case Strype::BindingType::GamepadAxis: str.append(magic_enum::enum_name((Strype::GamepadAxis)binding.Code)); break;

		default: STY_VERIFY(false, "Undefined InputBinding");
		}

		return fmt::format_to(ctx.out(), "{}", str);
	}
};