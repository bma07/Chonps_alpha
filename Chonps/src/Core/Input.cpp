#include "cepch.h"
#include "Input.h"

#include "Window.h"

#include "Platform/WindowAPI/glfwAPI/glfwInput.h"

namespace Chonps
{
	static Input* s_Input;

	void windowInputInit()
	{
		switch (getWindowContext())
		{
			case Chonps::WindowAPI::None:
			{
				CHONPS_CORE_WARN("No Windows API initialized!");
				break;
			}
			case Chonps::WindowAPI::Glfw:
			{
				s_Input = new glfwInput();
				break;
			}
			case Chonps::WindowAPI::Win32:
			{

				break;
			}
			default:
			{
				CHONPS_CORE_ERROR("ERROR: WINDOW: windowInputInit() - Cannot find the Windows API selected!");
				break;
			}
		}
	}

	bool keyPressed(Window* window, int keycode)
	{
		return s_Input->KeyPressed(window, keycode);
	}

	bool keyReleased(Window* window, int keycode)
	{
		return s_Input->KeyReleased(window, keycode);
	}

	bool mouseButtonPressed(Window* window, int button)
	{
		return s_Input->MouseButtonPressed(window, button);
	}

	bool mouseButtonReleased(Window* window, int button)
	{
		return s_Input->MouseButtonReleased(window, button);
	}

	vec2f getMousePos(Window* window)
	{
		return s_Input->GetMousePos(window);
	}

	void getMousePos(Window* window, float* xpos, float* ypos)
	{
		s_Input->GetMousePos(window, xpos, ypos);
	}

	float getMouseX(Window* window)
	{
		return s_Input->GetMouseX(window);
	}

	float getMouseY(Window* window)
	{
		return s_Input->GetMouseY(window);
	}

	void setMousePos(Window* window, float x, float y)
	{
		s_Input->SetMousePos(window, x, y);
	}

	vec2i getWindowPos(Window* window)
	{
		return s_Input->GetWindowPos(window);
	}

	void getWindowPos(Window* window, int* xpos, int* ypos)
	{
		s_Input->GetWindowPos(window, xpos, ypos);
	}

	vec2i getWindowSize(Window* window)
	{
		return s_Input->GetWindowSize(window);
	}

	void getWindowSize(Window* window, int* width, int* height)
	{
		s_Input->GetWindowSize(window, width, height);
	}

	void setMouseModeHide(Window* window, bool hide)
	{
		s_Input->SetMouseModeHide(window, hide);
	}

	float getTimeSeconds()
	{
		return s_Input->GetTimestep();
	}

	float getTimeMilliseconds()
	{
		return s_Input->GetTimestep() * 1000;
	}
}