#pragma once

#include "stypch.hpp"

#include "Strype/Core/Input.hpp"

namespace Strype {

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowMove, WindowDrop,
		MouseMoved, MouseScrolled,

		// Just a agnostic way of reprensting any input (doesnt involve Project)
		BindingPressed, BindingReleased,

		AssetImported, AssetRemoved, AssetMoved
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryWindow = BIT(1),
		EventCategoryInput = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryAsset = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

#define EVENT_CLASS_GLOBAL(enabled) virtual bool IsGlobal() const override { return enabled; }

	class Event
	{
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual bool IsGlobal() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) const
		{
			return GetCategoryFlags() & category;
		}
	private:
		bool Dispatched = false;
		
		friend class Application;
	};

	using EventQueue = std::deque<Event*>;

	template<typename T>
	using EventCallback = std::function<void(T&)>;

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
				func(*(T*)&m_Event);
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
		EVENT_CLASS_GLOBAL(true)
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
		EVENT_CLASS_GLOBAL(true)
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
		EVENT_CLASS_GLOBAL(true)
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
		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryWindow)
		EVENT_CLASS_GLOBAL(false)
	private:
		glm::vec2 m_Size;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryWindow)
		EVENT_CLASS_GLOBAL(false)
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
		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryWindow)
		EVENT_CLASS_GLOBAL(false)
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
		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryWindow)
		EVENT_CLASS_GLOBAL(false)
	private:
		std::vector<std::filesystem::path> m_Paths;
	};

	class BindingPressedEvent : public Event
	{
	public:
		template<typename T>
		BindingPressedEvent(BindingType type, T code)
			: m_Binding(type, (int)code) {}

		BindingPressedEvent(InputBinding binding)
			: m_Binding(binding) {}

		const InputBinding& GetBinding() const { return m_Binding; }

		std::string ToString() const override
		{
			return fmt::format("BindingPressedEvent: {}", m_Binding);
		}

		EVENT_CLASS_TYPE(BindingPressed)
		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryInput)
		EVENT_CLASS_GLOBAL(false)
	private:
		InputBinding m_Binding;
	};

	class BindingReleasedEvent : public Event
	{
	public:
		template<typename T>
		BindingReleasedEvent(BindingType type, T code)
			: m_Binding(type, (int)code) {}

		BindingReleasedEvent(InputBinding binding)
			: m_Binding(binding) {}

		const InputBinding& GetBinding() const { return m_Binding; }

		std::string ToString() const override
		{
			return fmt::format("BindingReleasedEvent: {}", m_Binding);
		}

		EVENT_CLASS_TYPE(BindingReleased)
		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryInput)
		EVENT_CLASS_GLOBAL(false)
	private:
		InputBinding m_Binding;
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
		EVENT_CLASS_GLOBAL(false)
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
		EVENT_CLASS_GLOBAL(false)
	private:
		glm::vec2 m_Offset;
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