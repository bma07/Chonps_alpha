#ifndef WINDOW_EVENTS_H
#define WINDOW_EVENTS_H

#include "Event.h"

namespace Chonps
{
	class CHONPS_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		static EventType GetStaticType() { return EventType::WindowClose; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowClose"; }
	};

	class CHONPS_API WindowResizeEvent : public Event
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

	class CHONPS_API WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		static EventType GetStaticType() { return EventType::WindowFocus; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowFocus"; }
	};

	class CHONPS_API WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() {}

		virtual int GetCategoryFlags() const override { return (EventCategoryInput | EventCategoryWindow); }

		static EventType GetStaticType() { return EventType::WindowLostFocus; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "WindowLostFocus"; }
	};

	class CHONPS_API WindowMovedEvent : public Event
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