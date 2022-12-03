#include "cepch.h"
#include "Window.h"

#include "Platform/WindowAPI/glfwAPI/glfwWindowAPI.h"

#include "Input.h"

namespace Chonps
{
	static WindowAPI s_WindowAPI = WindowAPI::None;

	bool setWindowContext(WindowAPI api /*= WindowAPI::Glfw*/)
	{
		s_WindowAPI = api;
		
		if (api == WindowAPI::None)
		{
			CHONPS_CORE_WARN("No Windows API selected!");
			return false;
		}

		switch (getWindowContext())
		{
			case Chonps::WindowAPI::None:
			{
				CHONPS_CORE_WARN("No Windows API initialized!");
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
				CHONPS_CORE_ERROR("ERROR: WINDOW: windowInitAPI(api) - Cannot find the Windows API selected!");
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
				CHONPS_CORE_WARN("No Windows API initialized!");
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
				CHONPS_CORE_ERROR("ERROR: WINDOW: windowTerminateAPI() - Cannot find the Windows API selected!");
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

		CHONPS_CORE_ERROR("ERROR: WINDOW: getWindowContextName() - Cannot find the Windows API selected!");
		return "null";
	}
}