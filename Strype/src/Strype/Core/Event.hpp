#pragma once

#include "stypch.hpp"

#include "Strype/Core/Input.hpp"

namespace Strype {

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowMove, WindowDrop,
		KeyPressed, KeyReleased, KeyHeld,
		MouseButtonPressed, MouseButtonReleased, MouseButtonHeld,
		MouseMoved, MouseScrolled,

		AssetImported, AssetRemoved, AssetMoved
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
		EventCategoryAsset = BIT(5)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event
	{
	public:
		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) const
		{
			return GetCategoryFlags() & category;
		}
	};

	template<typename T>
	using EventCallback = std::function<bool(T&)>;

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}

		template<typename T>
		bool Dispatch(EventCallback<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	class AssetImportedEvent : public Event
	{
	public:
		AssetImportedEvent(uint64_t handle)
			: m_Handle(handle) 
		{
		}

		uint64_t GetHandle() const { return m_Handle; }

		std::string ToString() const override
		{
			return fmt::format("AssetImportedEvent: {}", m_Handle);
		}

		EVENT_CLASS_TYPE(AssetImported)
		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryAsset)
	private:
		uint64_t m_Handle;
	};

	class AssetRemovedEvent : public Event
	{
	public:
		AssetRemovedEvent(uint64_t handle)
			: m_Handle(handle)
		{
		}

		uint64_t GetHandle() const { return m_Handle; }

		std::string ToString() const override
		{
			return fmt::format("AssetRemovedEvent: {}", m_Handle);
		}

		EVENT_CLASS_TYPE(AssetRemoved)
		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryAsset)
	private:
		uint64_t m_Handle;
	};

	class AssetMovedEvent : public Event
	{
	public:
		AssetMovedEvent(uint64_t handle, std::filesystem::path newpath)
			: m_Handle(handle), m_NewPath(newpath)
		{
		}

		uint64_t GetHandle() const { return m_Handle; }
		const std::filesystem::path& GetNewPath() const { return m_NewPath; }

		std::string ToString() const override
		{
			return fmt::format("AssetMovedEvent: {}", m_NewPath);
		}

		EVENT_CLASS_TYPE(AssetMoved)
		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryAsset)
	private:
		uint64_t m_Handle;
		std::filesystem::path m_NewPath;
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(glm::vec2 size)
			: m_Size(size) {}

		glm::vec2 GetSize() const { return m_Size; }

		std::string ToString() const override
		{
			return fmt::format("WindowResizeEvent: {}", m_Size);
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		glm::vec2 m_Size;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowMoveEvent : public Event
	{
	public:
		WindowMoveEvent(glm::vec2 position)
			: m_Position(position) {}

		glm::vec2 GetPosition() const { return m_Position; }

		std::string ToString() const override
		{
			return fmt::format("WindowMoveEvent: {}", m_Position);
		}

		EVENT_CLASS_TYPE(WindowMove)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		glm::vec2 m_Position;
	};

	class WindowDropEvent : public Event
	{
	public:
		WindowDropEvent(const std::vector<std::filesystem::path>& paths)
			: m_Paths(paths) {}

		const std::vector<std::filesystem::path>& GetPaths() const { return m_Paths; }

		EVENT_CLASS_TYPE(WindowDrop)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		std::vector<std::filesystem::path> m_Paths;
	};

	class KeyEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }

		std::string ToString() const override
		{
			return fmt::format("{}: {}", GetName(), (uint16_t)m_KeyCode);
		}

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(KeyCode keycode)
			: m_KeyCode(keycode) {}

		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode keycode)
			: KeyEvent(keycode) {}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyCode keycode)
			: KeyEvent(keycode) {}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyHeldEvent : public KeyEvent
	{
	public:
		KeyHeldEvent(KeyCode keycode)
			: KeyEvent(keycode) {}

		EVENT_CLASS_TYPE(KeyHeld)
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(glm::vec2 position)
			: m_Position(position) {}

		glm::vec2 GetPosition() const { return m_Position; }

		std::string ToString() const override
		{
			return fmt::format("MouseMovedEvent: {}", m_Position);
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		glm::vec2 m_Position;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(glm::vec2 offset)
			: m_Offset(offset) {}

		glm::vec2 GetOffset() const { return m_Offset; }

		std::string ToString() const override
		{
			return fmt::format("MouseScrolledEvent: {}", m_Offset);
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		glm::vec2 m_Offset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseCode GetMouseButton() const { return m_Button; }

		std::string ToString() const override
		{
			return fmt::format("{}: {}", GetName(), (uint16_t)m_Button);
		}

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtonEvent(MouseCode button)
			: m_Button(button) {}

		MouseCode m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseCode button)
			: MouseButtonEvent(button) {}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonHeldEvent : public MouseButtonEvent
	{
	public:
		MouseButtonHeldEvent(MouseCode button)
			: MouseButtonEvent(button) {}

		EVENT_CLASS_TYPE(MouseButtonHeld)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseCode button)
			: MouseButtonEvent(button) {}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}

template <>
struct fmt::formatter<Strype::Event>
{
	constexpr auto parse(format_parse_context& ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const Strype::Event& e, FormatContext& ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", e.ToString());
	}
};