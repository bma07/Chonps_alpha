#ifndef HG_CHONPS_WINDOW_H
#define HG_CHONPS_WINDOW_H

#include <functional>

#include "Events/Event.h"

namespace Chonps
{
	using EventCallbackFn = std::function<void(Event&)>;

	enum class WindowAPI
	{
		None = 0,
		Glfw,
		Win32
	};

	struct WindowIconData
	{
		unsigned char* image = nullptr;
		int width = 0, height = 0;
	};

	struct WindowData
	{
		uint32_t width, height;
		std::string title;
		WindowIconData* pIcons = nullptr;
		uint32_t iconCount = 0;
		bool fullscreen = false, resizable = true, vSync = true;
		int minWidth = 0, minHeight = 0;
		int maxWidth = -1, maxHeight = -1;

		EventCallbackFn EventCallback;
	};

	class Window
	{
	public:
		Window(uint32_t width, uint32_t height, std::string title, bool fullscreen, bool resizable, int minWidth, int minHeight) {}
		Window(WindowData winCreateInfo) {}

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
		virtual bool GetVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual void SetContextCurrent() = 0;

		virtual void Delete() = 0;
	};

	std::shared_ptr<Window> createWindowSp(WindowData winCreateInfo);
	Window* createWindow(WindowData winCreateInfo);

	std::shared_ptr<Window> createWindowSp(uint32_t width, uint32_t height, std::string title, bool fullscreen = false, bool resizable = true, int minWidth = 0, int minHeight = 0);
	Window* createWindow(uint32_t width, uint32_t height, std::string title, bool fullscreen = false, bool resizable = true, int minWidth = 0, int minHeight = 0);

	bool setWindowContext(WindowAPI api);
	bool windowTerminateContext();

	WindowAPI getWindowContext();
	std::string getWindowContextName();

	// All rendering will be done on the window that is set to the current render target
	void setWindowContextRenderTarget(Window* window);

	WindowIconData getWindowIconImageData(const std::string& filepath);
}


#endif