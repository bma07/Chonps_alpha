#ifndef WINDOW_H
#define WINDOW_H

#include "Core.h"
#include "Events/Event.h"

namespace Chonps
{
	class CHONPS_API Window
	{
	public:
		Window(std::string Title, int Width, int Height) {}

		using EventCallbackFn = std::function<void(Event&)>;

		virtual void OnUpdate() = 0;
		virtual void OnEvent(Event& e) = 0;
		virtual bool WindowIsOpen() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;


		// Window attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void LogEvents(bool enable) = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual void Delete() = 0;
	};

	Window* createWindow(std::string title, unsigned int width, unsigned int height, bool fullScreen = false);
	void initWindowAPI();
	void terminateWindowAPI();
}


#endif