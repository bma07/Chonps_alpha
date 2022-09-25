#ifndef GLFW_API_H
#define GLFW_API_H

#include "Core.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Chonps
{
	class CHONPS_API glfwWindowAPI : public Window
	{
	public:
		glfwWindowAPI(std::string Title, int Width, int Height, bool fullScreen = false);

		using EventCallbackFn = std::function<void(Event&)>;

		virtual void OnUpdate() override;
		virtual void OnEvent(Event& e) override;
		virtual bool WindowIsOpen() override;

		virtual unsigned int GetWidth() const { return m_Data.Width; }
		virtual unsigned int GetHeight() const { return m_Data.Height; }


		// Window attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
		virtual void LogEvents(bool enable) { m_LogEvents = enable; }

		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const { return m_Data.VSync; }

		virtual void* GetNativeWindow() const { return m_Window; }

		virtual void Delete() override;

	private:
		void Create();

		struct WindowData
		{
			std::string Title;
			int Width, Height;
			bool FullScreen;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;

		GLFWwindow* m_Window;

		bool m_LogEvents = false;
	};
}


#endif