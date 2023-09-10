#ifndef HG_CHONPS_WINDOW_EVENTS_H
#define HG_CHONPS_WINDOW_EVENTS_H

#include "Event.h"

namespace Chonps
{
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		static EventType GetStaticType() { return EventType::WindowClose; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowClose"; }
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(int width, int height) 
			: m_Width(width), m_Height(height) {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: (" << m_Width << ", " << m_Height << ")";
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WindowResize; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowResize"; }

	private:
		int m_Width, m_Height;

	};

	class WindowFramebufferResizeEvent : public Event
	{
	public:
		WindowFramebufferResizeEvent(int width, int height)
			: m_Width(width), m_Height(height) {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowFramebufferResizeEvent: (" << m_Width << ", " << m_Height << ")";
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WindowFramebufferResize; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowFramebufferResize"; }

	private:
		int m_Width, m_Height;

	};

	class WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		static EventType GetStaticType() { return EventType::WindowFocus; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowFocus"; }
	};

	class WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		static EventType GetStaticType() { return EventType::WindowLostFocus; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowLostFocus"; }
	};

	class WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(int x, int y) 
			: m_WindowX(x), m_WindowY(y) {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: (" << m_WindowX << ", " << m_WindowY << ")";
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WindowMoved; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowMoved"; }

	private:
		int m_WindowX, m_WindowY;
	};
}

#endif