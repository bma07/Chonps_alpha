#include "cepch.h"
#include "glfwWindowAPI.h"

#include "Graphics/RendererAPI.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

#include "Core/Events/KeyEvents.h"
#include "Core/Events/MouseEvents.h"
#include "Core/Events/WindowEvents.h"

#include <stb_image.h>

namespace Chonps
{
	static bool s_glfwInit = false;

	static void GLFWerrorCallback(int error, const char* descripion)
	{
		CHONPS_CORE_ERROR("GLFW ERROR ({0}): {1}", error, descripion);
	}

	void glfwInitWindowContext()
	{
		if (!s_glfwInit)
		{
			int success = glfwInit();
			CHONPS_CORE_ASSERT(success, "Failed to initialize glfw");
			glfwSetErrorCallback(GLFWerrorCallback);
			s_glfwInit = true;
		}
		else CHONPS_CORE_WARN("glfw already initialized");
	}

	void glfwTerminateWindowContext()
	{
		if (s_glfwInit)
		{
			glfwTerminate();
			s_glfwInit = false;
		}
		else
			CHONPS_CORE_WARN("glfw already terminated!");
	}

	void glfwImplSetWindowContextRenderTarget(Window* window)
	{
		switch (getGraphicsAPI())
		{
			case Chonps::GraphicsAPI::None:
			{
				CHONPS_CORE_ASSERT(false, "No graphics API selected beforehand! Cannot create window without a rendering context!");
			}

			case Chonps::GraphicsAPI::OpenGL:
			{
				gladInit(static_cast<GLFWwindow*>(window->GetNativeWindow()), window->GetWidth(), window->GetHeight());
				break;
			}

			case Chonps::GraphicsAPI::Vulkan:
			{
				setCurrentWindowForVulkanWindowSurface(static_cast<GLFWwindow*>(window->GetNativeWindow()));
				break;
			}

			case Chonps::GraphicsAPI::DirectX:
			{
				break;
			}

			default:
			{
				CHONPS_CORE_ERROR("Cannot find the graphics API selected!");
				break;
			}
		}
	}

	glfwWindowAPI::glfwWindowAPI(uint32_t width, uint32_t height, std::string title, bool fullscreen, bool resizable, int minWidth, int minHeight)
		: Window(width, height, title, fullscreen, resizable, minWidth, minHeight)
	{
		m_Data.title = title;
		m_Data.width = width;
		m_Data.height = height;
		m_Data.fullscreen = fullscreen;
		m_Data.resizable = resizable;
		m_Data.minWidth = minWidth;
		m_Data.minHeight = minHeight;

		Create();
		SetEventCallback(std::bind(&Window::OnEvent, this, std::placeholders::_1));
	}

	glfwWindowAPI::glfwWindowAPI(WindowData winCreateInfo)
		: Window(winCreateInfo), m_Data(winCreateInfo)
	{
		Create();
		SetEventCallback(std::bind(&Window::OnEvent, this, std::placeholders::_1));
	}

	void glfwWindowAPI::Create()
	{
		GLFWmonitor* fullScreen = nullptr;
		if (m_Data.fullscreen)
			fullScreen = glfwGetPrimaryMonitor();

		GraphicsAPI graphicsAPI = getGraphicsAPI();
		if (graphicsAPI == GraphicsAPI::Vulkan)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		if (!m_Data.resizable)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		CHONPS_CORE_INFO("Creating Window: {0} ({1}, {2})", m_Data.title, m_Data.width, m_Data.height);
		m_Window = glfwCreateWindow(m_Data.width, m_Data.height, m_Data.title.c_str(), fullScreen, nullptr);
		CHONPS_CORE_ASSERT(m_Window, "Window failed to load!");

		if (m_Data.pIcons != nullptr)
		{
			std::vector<GLFWimage> images;
			for (uint32_t i = 0; i < m_Data.iconCount; i++)
			{
				GLFWimage image{};
				image.pixels = m_Data.pIcons[i].image;
				image.width = m_Data.pIcons[i].width;
				image.height = m_Data.pIcons[i].height;
				images.push_back(image);
			}

			glfwSetWindowIcon(m_Window, static_cast<int>(images.size()), images.data());
		}
		
		glfwSetWindowSizeLimits(m_Window, m_Data.minWidth, m_Data.minHeight, m_Data.maxWidth, m_Data.maxHeight);

		glfwSetWindowUserPointer(m_Window, &m_Data);

		// Set GLFW Callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent eventType(width, height);
			data.EventCallback(eventType);

			switch (getGraphicsAPI())
			{
				case GraphicsAPI::OpenGL: 
				{
					gladUpdateViewPort(window, width, height);
					break;
				}
				case GraphicsAPI::Vulkan:
				{
					getVulkanBackends()->framebufferResized = true;
					break;
				}
			}
		});

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;
			
			WindowFramebufferResizeEvent eventType(width, height);
			data.EventCallback(eventType);

			switch (getGraphicsAPI())
			{
				case GraphicsAPI::OpenGL:
				{
					gladUpdateViewPort(window, width, height);
					break;
				}
				case GraphicsAPI::Vulkan:
				{
					getVulkanBackends()->framebufferResized = true;
					break;
				}
			}
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
		if (getGraphicsAPI() == GraphicsAPI::OpenGL) glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void glfwWindowAPI::OnEvent(Event& e)
	{
		if (e.IsInCategory(EventCategoryInput) && m_LogEvents)
		{
			CHONPS_CORE_INFO("Window '{0}': {1}", m_Data.title, e.ToString());
		}
	}

	bool glfwWindowAPI::WindowIsOpen()
	{
		return (!glfwWindowShouldClose(m_Window));
	}

	void glfwWindowAPI::SetContextCurrent()
	{
		if (getGraphicsAPI() == GraphicsAPI::OpenGL)
			glfwMakeContextCurrent(m_Window);
	}

	void glfwWindowAPI::SetVSync(bool enabled)
	{
		if (getGraphicsAPI() == GraphicsAPI::OpenGL)
		{
			glfwSwapInterval(enabled);
			m_Data.vSync = enabled;
		}
	}

	void glfwWindowAPI::Delete()
	{
		if (getGraphicsAPI() == GraphicsAPI::Vulkan)
			vkDeviceWaitIdle(getVulkanBackends()->device);

		glfwDestroyWindow(m_Window);
	}
}