#pragma once

#include "stypch.hpp"

#include "Strype/Core/Base.hpp"
#include "Strype/Events/Event.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#if defined(AGI_WINDOWS)
#	define GLFW_EXPOSE_NATIVE_WIN32
#	include <GLFW/glfw3native.h>
#elif defined(AGI_MACOSX)
#	define GLFW_EXPOSE_NATIVE_COCOA
#	include <GLFW/glfw3native.h>
#elif defined(AGI_LINUX)
#	define GLFW_EXPOSE_NATIVE_X11
#	define GLFW_EXPOSE_NATIVE_WAYLAND
#	include <GLFW/glfw3native.h>
#endif

namespace Strype {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		bool VSync;
		bool Maximise;

		WindowProps(const std::string& title = "Strype Engine",
			unsigned int width = 1280,
			unsigned int height = 720,
			bool vsync = true,
			bool maximise = false)
			: Title(title), Width(width), Height(height), VSync(vsync), Maximise(maximise)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props);
		~Window();

		void OnUpdate();

		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline unsigned int GetHeight() const { return m_Data.Height; }
		glm::vec2 GetPosition() const;

		// Window attributes
		void SetVSync(bool enabled);
		bool IsVSync() const;
		void SetVisable(bool enabled);
		void SetTitle(const std::string& title);

		GLFWwindow* GetNativeWindow() const { return m_Window; }
		void* GetOsWindow() const;

		static inline Scope<Window> Create(const WindowProps& props = WindowProps()) { return CreateScope<Window>(props); }
	private:
		GLFWwindow* m_Window;
		WindowProps m_Data;
	};

}