#include "cepch.h"
#include "Window.h"

#ifdef CHONPS_GLFW_API
	#define GLFW_INCLUDE_NONE
	#include <GLFW/glfw3.h>
	#include "Platform/OpenGL/OpenGLContext.h"
#endif

#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"

//#include "Platform/OpenGL/OpenGLContext.h"

namespace Chonps
{



	static bool s_glfwInit = false;

	static void GLFWerrorCallback(int error, const char* descripion)
	{
		CHONPS_CORE_ERROR("GLFW ERROR ({0}): {1}", error, descripion);
	}

	Window::Window(std::string Title, int Width, int Height)
	{
		m_Data.Title = Title;
		m_Data.Width = Width;
		m_Data.Height = Height;
		Init();
		SetEventCallback(std::bind(&Chonps::Window::OnEvent, this, std::placeholders::_1));
	}

	void Window::Init()
	{
		if (!s_glfwInit)
		{
			int success = glfwInit();
			CHONPS_CORE_ASSERT(success, "Failed to initialize glfw!");
			glfwSetErrorCallback(GLFWerrorCallback);
			s_glfwInit = true;
		}

		CHONPS_CORE_INFO("Creating Window: {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);
		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		CHONPS_CORE_ASSERT(m_Window, "Window failed to load!");

		gladInit(m_Window);

		glfwSetWindowUserPointer(m_Window, &m_Data);
		glfwSwapInterval(true);
		
		// Set GLFW Callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, false);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

	}

	void Window::OnUpdate()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void Window::OnEvent(Event& e)
	{
		CHONPS_CORE_INFO(e.ToString());
	}

	void Window::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled);
		m_Data.VSync = enabled;
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void Window::Terminate()
	{
		glfwTerminate();
	}

}