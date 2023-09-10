#include "cepch.h"
#include "glfwInput.h"

#include "Core/Window.h"
#include <GLFW/glfw3.h>

static float s_ScrollX = 0.0f, s_ScrollY = 0.0f;

namespace Chonps
{
	bool glfwInput::KeyPressed(Window* window, int keycode)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int state = glfwGetKey(glfwWin, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool glfwInput::KeyReleased(Window* window, int keycode)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int state = glfwGetKey(glfwWin, keycode);
		return state == GLFW_RELEASE;
	}

	bool glfwInput::MouseButtonPressed(Window* window, int button)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int state = glfwGetMouseButton(glfwWin, button);
		return state == GLFW_PRESS;
	}

	bool glfwInput::MouseButtonReleased(Window* window, int button)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int state = glfwGetMouseButton(glfwWin, button);
		return state == GLFW_RELEASE;
	}

	pair<float, float> glfwInput::GetMousePos(Window* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(glfwWin, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	void glfwInput::GetMousePos(Window* window, float* xpos, float* ypos)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		double xPos = (double)*xpos, yPos = (double)*ypos;
		glfwGetCursorPos(glfwWin, &xPos, &yPos);
		*xpos = (float)xPos;
		*ypos = (float)yPos;
	}

	float glfwInput::GetMouseX(Window* window)
	{
		pair<float, float> MousePos = getMousePos(window);
		return MousePos.x;
	}

	float glfwInput::GetMouseY(Window* window)
	{
		pair<float, float> MousePos = getMousePos(window);
		return MousePos.y;
	}

	void glfwInput::SetMousePos(Window* window, float x, float y)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		glfwSetCursorPos(glfwWin, (double)x, (double)y);
	}

	pair<int, int> glfwInput::GetWindowPos(Window* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int xpos, ypos;
		glfwGetWindowPos(glfwWin, &xpos, &ypos);
		return { xpos, ypos };
	}

	void glfwInput::GetWindowPos(Window* window, int* xpos, int* ypos)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		glfwGetWindowPos(glfwWin, &(*xpos), &(*ypos));
	}

	pair<int, int> glfwInput::GetWindowSize(Window* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int width, height;
		glfwGetWindowSize(glfwWin, &width, &height);
		return { width, height };
	}

	void glfwInput::GetWindowSize(Window* window, int* width, int* height)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		glfwGetWindowSize(glfwWin, &(*width), &(*height));
	}

	void glfwInput::SetMouseModeHide(Window* window, bool hide)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int mode = hide ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL;
		glfwSetInputMode(glfwWin, GLFW_CURSOR, mode);
	}

	float glfwInput::GetTimestep()
	{
		return (float)glfwGetTime();
	}
}
