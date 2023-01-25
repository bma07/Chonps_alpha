#ifndef CHONPS_GLFW_API_H
#define CHONPS_GLFW_API_H

#include "Core/Core.h"
#include "Core/Window.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Chonps
{
	void glfwInitWindowContext();
	void glfwTerminateWindowContext();

	class glfwWindowAPI : public Window
	{
	public:
		glfwWindowAPI(std::string Title, int Width, int Height, bool fullScreen = false);

		using EventCallbackFn = std::function<void(Event&)>;

		virtual void OnUpdate() override;
		virtual void OnEvent(Event& e) override;
		virtual bool WindowIsOpen() override;

		virtual unsigned int GetWidth() const override { return m_Data.Width; }
		virtual unsigned int GetHeight() const { return m_Data.Height; }


		// Window attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void LogEvents(bool enable) override { m_LogEvents = enable; }

		virtual void SetVSync(bool enabled) override;
		virtual bool GetVSync() const override { return m_Data.VSync; }

		virtual void* GetNativeWindow() const override { return m_Window; }
		virtual void SetContextCurrent() override;

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