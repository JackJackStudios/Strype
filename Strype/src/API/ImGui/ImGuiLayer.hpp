#pragma once

#include "Strype/Core/Layer.hpp"

#include "Strype/Events/ApplicationEvent.hpp"
#include "Strype/Events/KeyEvent.hpp"
#include "Strype/Events/MouseEvent.hpp"

namespace Strype {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(bool dockspace = false);
		~ImGuiLayer();

		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
		bool m_Dockspace;
	};

}