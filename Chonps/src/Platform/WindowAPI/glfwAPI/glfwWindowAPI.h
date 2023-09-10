#ifndef HG_CHONPS_GLFW_API_H
#define HG_CHONPS_GLFW_API_H

#include "Core/Core.h"
#include "Core/Window.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Chonps
{
	void glfwInitWindowContext();
	void glfwTerminateWindowContext();

	void glfwImplSetWindowContextRenderTarget(Window* window);

	class glfwWindowAPI : public Window
	{
	public:
		glfwWindowAPI(uint32_t width, uint32_t height, std::string title, bool fullscreen, bool resizable, int minWidth, int minHeight);
		glfwWindowAPI(WindowData winCreateInfo);

		virtual void OnUpdate() override;
		virtual void OnEvent(Event& e) override;
		virtual bool WindowIsOpen() override;

		virtual unsigned int GetWidth() const override { return m_Data.width; }
		virtual unsigned int GetHeight() const { return m_Data.height; }


		// Window attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void LogEvents(bool enable) override { m_LogEvents = enable; }

		virtual void SetVSync(bool enabled) override;
		virtual bool GetVSync() const override { return m_Data.vSync; }

		virtual void* GetNativeWindow() const override { return m_Window; }
		virtual void SetContextCurrent() override;

		virtual void Delete() override;

	private:
		void Create();

		WindowData m_Data;
		GLFWwindow* m_Window;
		bool m_LogEvents = false;
	};
}


#endif