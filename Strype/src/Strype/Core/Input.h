#pragma once

#include "Strype/Core/KeyCodes.h"
#include "Strype/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace Strype {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}