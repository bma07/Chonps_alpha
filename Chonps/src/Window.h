#ifndef WINDOW_H
#define WINDOW_H

#include "Core.h"
#include "Events/Event.h"

#ifdef CHONPS_USE_WIN32
	#define CHONPS_WIN32_API
#else
	#define CHONPS_GLFW_API
#endif

#ifdef CHONPS_GLFW_API
	struct GLFWwindow;
	#define WINDOW_API GLFWwindow*
#endif

namespace Chonps
{
	class CHONPS_API Window
	{
	public:
		Window(std::string Title, int Width, int Height);

		using EventCallbackFn = std::function<void(Event&)>;

		void Init();
		void OnUpdate();
		void OnEvent(Event& e);

		unsigned int GetWidth() const { return m_Data.Width; }
		unsigned int GetHeight() const { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync() const { return m_Data.VSync; }

		void* GetNativeWindow() const { return m_Window; }

		void Shutdown();
		void Terminate();

	private:
		struct WindowData
		{
			std::string Title;
			int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;

		WINDOW_API m_Window;

	};
}


#endif