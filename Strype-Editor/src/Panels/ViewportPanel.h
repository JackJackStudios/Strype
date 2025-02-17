#pragma once

#include "EditorPanel.h"

namespace Strype {

	using ResizeCallbackFunc = std::function<void(float, float)>;
	using DrawCallbackFunc = std::function<void(Timestep)>;

	class ViewportPanel : public EditorPanel
	{
	public:
		ViewportPanel();

		virtual void OnImGuiRender();
		virtual void OnEvent(Event& event) {};
		inline virtual void SetRoomContext(const Ref<Room>& room) { m_ActiveScene = room; }

		void OnUpdate(Timestep ts);
		Ref<Framebuffer>& GetFramebuffer() { return m_Framebuffer; }

		void SetResizedCallback(ResizeCallbackFunc func) { m_ResizeCallback = func; }
		void SetDrawCallback(DrawCallbackFunc func) { m_DrawCallback = func; }

	private:
		Ref<Room> m_ActiveScene;
		Ref<Framebuffer> m_Framebuffer;

		ResizeCallbackFunc m_ResizeCallback;
		DrawCallbackFunc m_DrawCallback;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	};

}