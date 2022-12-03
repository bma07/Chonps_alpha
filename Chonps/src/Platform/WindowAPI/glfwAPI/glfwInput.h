#ifndef CHONPS_GLFW_INPUT_H
#define CHONPS_GLFW_INPUT_H

#include "Core/Input.h"

namespace Chonps
{
	class glfwInput : public Input
	{
	public:
		virtual bool KeyPressed(Window* window, int keycode) override;
		virtual bool KeyReleased(Window* window, int keycode) override;
		virtual bool MouseButtonPressed(Window* window, int button) override;
		virtual bool MouseButtonReleased(Window* window, int button) override;

		virtual vec2f GetMousePos(Window* window) override;
		virtual void GetMousePos(Window* window, float* xpos, float* ypos) override;
		virtual float GetMouseX(Window* window) override;
		virtual float GetMouseY(Window* window) override;

		virtual void SetMousePos(Window* window, float x, float y) override;

		virtual vec2i GetWindowPos(Window* window) override;
		virtual void GetWindowPos(Window* window, int* xpos, int* ypos) override;
		virtual vec2i GetWindowSize(Window* window) override;
		virtual void GetWindowSize(Window* window, int* width, int* height) override;

		virtual void SetMouseModeHide(Window* window, bool hide) override;

		virtual float GetTimestep() override;
	};
}

#endif