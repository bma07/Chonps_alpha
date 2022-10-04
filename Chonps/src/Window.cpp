#include "cepch.h"
#include "Window.h"

namespace Chonps
{
	static WindowAPI s_WindowAPI = WindowAPI::None;

	bool windowInitAPI(WindowAPI api /*= WindowAPI::Glfw*/)
	{
		if (api == WindowAPI::None)
		{
			s_WindowAPI = api;
			CHONPS_CORE_WARN("No Windows API selected!");
			return false;
		}
		s_WindowAPI = api;

		switch (getWindowContext())
		{
			case Chonps::WindowAPI::None:
			{
				CHONPS_CORE_WARN("No Windows API initialized!");
				break;
			}
			case Chonps::WindowAPI::Glfw:
			{
				glfwInitWindowAPI();
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

		CHONPS_CORE_INFO("Window Context Initialized: {0}", getWindowContextName());
		return true;
	}

	bool windowTerminateAPI()
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
				glfwTerminateWindowAPI();
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