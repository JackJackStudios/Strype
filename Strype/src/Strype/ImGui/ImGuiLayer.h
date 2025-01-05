#pragma once

#include "Strype/Core/Layer.h"

#include "Strype/Events/ApplicationEvent.h"
#include "Strype/Events/KeyEvent.h"
#include "Strype/Events/MouseEvent.h"

namespace Strype {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
	};

}