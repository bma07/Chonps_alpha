#ifndef APPLICATION_EVENTS_H
#define APPLICATION_EVENTS_H

#include "Event.h"

namespace Chonps
{
	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }

		static EventType GetStaticType() { return EventType::AppTick; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "AppTick"; }
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }

		static EventType GetStaticType() { return EventType::AppRender; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "AppRender"; }
	};
}

#endif