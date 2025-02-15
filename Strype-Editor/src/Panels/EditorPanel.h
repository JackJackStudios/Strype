#pragma once

#include <Strype.h>

namespace Strype {

	class EditorPanel
	{
	public:
		virtual ~EditorPanel() = default;

		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
		virtual void SetRoomContext(const Ref<Room>& room) = 0;
	};

}