#include "cepch.h"
#include "glfwWindowAPI.h"

#ifdef CHONPS_OPENGL_API
	#include "Platform/OpenGL/OpenGLContext.h"
#endif

#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"


namespace Chonps
{
	static bool s_glfwInit = false;

	static void GLFWerrorCallback(int error, const char* descripion)
	{
		CHONPS_CORE_ERROR("GLFW ERROR ({0}): {1}", error, descripion);
	}

#ifdef CHONPS_GLFW_API

	void initWindowAPI()
	{
		if (!s_glfwInit)
		{
			int success = glfwInit();
			CHONPS_CORE_ASSERT(success, "Failed to initialize glfw");
			glfwSetErrorCallback(GLFWerrorCallback);
			s_glfwInit = true;
		}
		else
			CHONPS_CORE_WARN("glfw already initialized");
	}

	void terminateWindowAPI()
	{
		if (s_glfwInit)
		{
			glfwTerminate();
			s_glfwInit = false;
		}
		else
			CHONPS_CORE_WARN("glfw already terminated!");
	}

#endif

	Window* createWindow(std::string title, unsigned int width, unsigned int height, bool fullScreen)
	{
		return new glfwWindowAPI(title, width, height, fullScreen);
	}

	glfwWindowAPI::glfwWindowAPI(std::string Title, int Width, int Height, bool fullScreen)
		: Window(Title, Width, Height)
	{
		m_Data.Title = Title;
		m_Data.Width = Width;
		m_Data.Height = Height;
		m_Data.FullScreen = fullScreen;
		Create();
		SetEventCallback(std::bind(&Chonps::Window::OnEvent, this, std::placeholders::_1));
	}

	void glfwWindowAPI::Create()
	{
		GLFWmonitor* fullScreen = nullptr;
		if (m_Data.FullScreen)
			fullScreen = glfwGetPrimaryMonitor();

		CHONPS_CORE_INFO("Creating Window: {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);
		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), fullScreen, nullptr);
		CHONPS_CORE_ASSERT(m_Window, "Window failed to load!");

		gladInit(m_Window);

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW Callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent eventType(width, height);
				data.EventCallback(eventType);
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowMovedEvent eventType(x, y);
				data.EventCallback(eventType);
			});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				if (focused)
				{
					WindowFocusEvent eventType;
					data.EventCallback(eventType);
				}
				else
				{
					WindowLostFocusEvent eventType;
					data.EventCallback(eventType);
				}
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent eventType;
				data.EventCallback(eventType);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent eventType(key, false);
					data.EventCallback(eventType);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent eventType(key);
					data.EventCallback(eventType);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent eventType(key, true);
					data.EventCallback(eventType);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent eventType(keycode);
				data.EventCallback(eventType);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent eventType(button);
					data.EventCallback(eventType);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent eventType(button);
					data.EventCallback(eventType);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent eventType((float)xOffset, (float)yOffset);
				data.EventCallback(eventType);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent eventType((float)xPos, (float)yPos);
				data.EventCallback(eventType);
			});

	}

	void glfwWindowAPI::OnUpdate()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void glfwWindowAPI::OnEvent(Event& e)
	{
		if (e.IsInCategory(EventCategoryInput) && m_LogEvents)
		{
			CHONPS_CORE_INFO("Window '{0}': {1}", m_Data.Title, e.ToString());
		}
	}

	bool glfwWindowAPI::WindowIsOpen()
	{
		return (!glfwWindowShouldClose(m_Window));
	}

	void glfwWindowAPI::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled);
		m_Data.VSync = enabled;
	}

	void glfwWindowAPI::Delete()
	{
		glfwDestroyWindow(m_Window);
	}
}