#include "cepch.h"

#ifdef CHONPS_GLFW_API
	#include "Input.h"
#endif

#include "Window.h"
#include "GLFW/glfw3.h"

namespace Chonps
{

#ifdef CHONPS_GLFW_API

	bool keyPressed(Window* window, int keycode)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int state = glfwGetKey(glfwWin, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool mouseButtonPressed(Window* window, int button)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int state = glfwGetMouseButton(glfwWin, button);
		return state == GLFW_PRESS;
	}

	vec2f getMousePos(Window* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(glfwWin, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	void getMousePos(Window* window, float* xpos, float* ypos)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		double xPos = (double)*xpos, yPos = (double)*ypos;
		glfwGetCursorPos(glfwWin, &xPos, &yPos);
		*xpos = (float)xPos;
		*ypos = (float)yPos;
	}

	float getMouseX(Window* window)
	{
		vec2f MousePos = getMousePos(window);
		return MousePos.x;
	}

	float getMouseY(Window* window)
	{
		vec2f MousePos = getMousePos(window);
		return MousePos.y;
	}

	vec2i getWindowPos(Window* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int xpos, ypos;
		glfwGetWindowPos(glfwWin, &xpos, &ypos);
		return { xpos, ypos };
	}

	void getWindowPos(Window* window, int* xpos, int* ypos)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		glfwGetWindowPos(glfwWin, &(*xpos), &(*ypos));
	}

	vec2i getWindowSize(Window* window)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		int width, height;
		glfwGetWindowSize(glfwWin, &width, &height);
		return { width, height };
	}

	void getWindowSize(Window* window, int* width, int* height)
	{
		GLFWwindow* glfwWin = static_cast<GLFWwindow*>(window->GetNativeWindow());
		glfwGetWindowSize(glfwWin, &(*width), &(*height));
	}

#endif

}
