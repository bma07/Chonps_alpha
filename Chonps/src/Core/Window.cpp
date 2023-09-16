#include "cepch.h"
#include "Window.h"

#include "Platform/WindowAPI/glfwAPI/glfwWindowAPI.h"
#include "Input.h"

#include <stb_image.h>

namespace Chonps
{
	static WindowAPI s_WindowAPI = WindowAPI::None;

	bool setWindowContext(WindowAPI api)
	{
		s_WindowAPI = api;
		
		if (api == WindowAPI::None)
		{
			CHONPS_CORE_LOG_WARN(Window, "lected!");
			return false;
		}

		switch (getWindowContext())
		{
			case Chonps::WindowAPI::None:
			{
				CHONPS_CORE_LOG_WARN(Window, "itialized!");
				break;
			}
			case Chonps::WindowAPI::Glfw:
			{
				glfwInitWindowContext();
				break;
			}
			case Chonps::WindowAPI::Win32:
			{
				
				break;
			}
			default:
			{
				CHONPS_CORE_LOG_ERROR(Window, "windowInitAPI(api) - Cannot find the Windows API selected!");
				break;
			}
		}

		windowInputInit();

		CHONPS_CORE_INFO("Window Context Initialized: {0}", getWindowContextName());
		return true;
	}

	bool windowTerminateContext()
	{
		switch (getWindowContext())
		{
			case Chonps::WindowAPI::None:
			{
				CHONPS_CORE_LOG_WARN(Window, "itialized!");
				return false;
			}
			case Chonps::WindowAPI::Glfw:
			{
				glfwTerminateWindowContext();
				break;
			}
			case Chonps::WindowAPI::Win32:
			{

				break;
			}
			default:
			{
				CHONPS_CORE_LOG_ERROR(Window, "windowTerminateAPI() - Cannot find the Windows API selected!");
				return false;
			}
		}

		s_WindowAPI = WindowAPI::None;
		CHONPS_CORE_INFO("Window Context Terminated: {0}", getWindowContextName());
		return true;
	}

	WindowAPI getWindowContext()
	{
		return s_WindowAPI;
	}

	std::string getWindowContextName()
	{
		switch (getWindowContext())
		{
			case Chonps::WindowAPI::None: return "None";
			case Chonps::WindowAPI::Glfw: return "glfw";
			case Chonps::WindowAPI::Win32: return "WIN32";
		}

		CHONPS_CORE_LOG_ERROR(Window, "getWindowContextName() - Cannot find the Windows API selected!");
		return "null";
	}

	void setWindowContextRenderTarget(Window* window)
	{
		switch (getWindowContext())
		{
			case WindowAPI::None:
			{
				CHONPS_CORE_LOG_WARN(Window, "setWindowContextRenderTarget() - No graphics API selected beforehand!");
				break;
			}
			case WindowAPI::Glfw:
			{
				glfwImplSetWindowContextRenderTarget(window);
				break;
			}
			case WindowAPI::Win32:
			{
				break;
			}
		}
	}

	WindowIconData getWindowIconImageData(const std::string& filepath)
	{
		int width, height, channels;
		unsigned char* pixels = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		
		WindowIconData icon{};
		icon.image = pixels;
		icon.width = width;
		icon.height = height;

		return icon;
	}

	std::shared_ptr<Window> createWindowSp(WindowData winCreateInfo)
	{
		switch (getWindowContext())
		{
			case WindowAPI::None:
			{
				CHONPS_CORE_LOG_WARN(Window, "createWindowSp() - No graphics API selected beforehand!");
				break;
			}

			case WindowAPI::Glfw: { return std::make_shared<glfwWindowAPI>(winCreateInfo); }

			case WindowAPI::Win32:
			{
				break;
			}
		}

		CHONPS_CORE_LOG_ERROR(Window, "Could not create Window!");
		return nullptr;
	}

	Window* createWindow(WindowData winCreateInfo)
	{
		switch (getWindowContext())
		{
			case WindowAPI::None:
			{
				CHONPS_CORE_LOG_WARN(Window, "createWindow() - No graphics API selected beforehand!");
				break;
			}

			case WindowAPI::Glfw: { return new glfwWindowAPI(winCreateInfo); }

			case WindowAPI::Win32:
			{
				break;
			}
		}

		CHONPS_CORE_LOG_ERROR(Window, "Could not create Window!");
		return nullptr;
	}

	std::shared_ptr<Window> createWindowSp(uint32_t width, uint32_t height, std::string title, bool fullscreen, bool resizable, int minWidth, int minHeight)
	{
		switch (getWindowContext())
		{
			case WindowAPI::None:
			{
				CHONPS_CORE_LOG_WARN(Window, "createWindowSp() - No graphics API selected beforehand!");
				break;
			}

			case WindowAPI::Glfw: { return std::make_shared<glfwWindowAPI>(width, height, title, fullscreen, resizable, minWidth, minHeight); }

			case WindowAPI::Win32:
			{
				break;
			}
		}

		CHONPS_CORE_LOG_ERROR(Window, "Could not create Window!");
		return nullptr;
	}

	Window* createWindow(uint32_t width, uint32_t height, std::string title, bool fullscreen, bool resizable, int minWidth, int minHeight)
	{
		switch (getWindowContext())
		{
			case WindowAPI::None:
			{
				CHONPS_CORE_LOG_WARN(Window, "createWindow() - No graphics API selected beforehand!");
				break;
			}

			case WindowAPI::Glfw: { return new glfwWindowAPI(width, height, title, fullscreen, resizable, minWidth, minHeight); }

			case WindowAPI::Win32:
			{
				break;
			}
		}

		CHONPS_CORE_LOG_ERROR(Window, "Could not create Window!");
		return nullptr;
	}
}