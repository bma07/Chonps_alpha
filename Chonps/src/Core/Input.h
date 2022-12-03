#ifndef CHONPS_INPUT_H
#define CHONPS_INPUT_H

#include "Core.h"
#include "Window.h"
#include "DataTypes.h"

namespace Chonps
{
	void windowInputInit();

	class Input
	{
	public:
		virtual bool KeyPressed(Window* window, int keycode) = 0;
		virtual bool KeyReleased(Window* window, int keycode) = 0;
		virtual bool MouseButtonPressed(Window* window, int button) = 0;
		virtual bool MouseButtonReleased(Window* window, int button) = 0;

		virtual vec2f GetMousePos(Window* window) = 0;
		virtual void GetMousePos(Window* window, float* xpos, float* ypos) = 0;
		virtual float GetMouseX(Window* window) = 0;
		virtual float GetMouseY(Window* window) = 0;

		virtual void SetMousePos(Window* window, float x, float y) = 0;

		virtual vec2i GetWindowPos(Window* window) = 0;
		virtual void GetWindowPos(Window* window, int* xpos, int* ypos) = 0;
		virtual vec2i GetWindowSize(Window* window) = 0;
		virtual void GetWindowSize(Window* window, int* width, int* height) = 0;

		virtual void SetMouseModeHide(Window* window, bool hide) = 0;

		virtual float GetTimestep() = 0;
	};

	// Input Functions
	bool keyPressed(Window* window, int keycode);
	bool keyReleased(Window* window, int keycode);
	bool mouseButtonPressed(Window* window, int button);
	bool mouseButtonReleased(Window* window, int button);

	vec2f getMousePos(Window* window);
	void getMousePos(Window* window, float* xpos, float* ypos);
	float getMouseX(Window* window);
	float getMouseY(Window* window);

	void setMousePos(Window* window, float x, float y);

	vec2i getWindowPos(Window* window);
	void getWindowPos(Window* window, int* xpos, int* ypos);
	vec2i getWindowSize(Window* window);
	void getWindowSize(Window* window, int* width, int* height);

	void setMouseModeHide(Window* window, bool hide);

	// Timestep Functions
	float getTimeSeconds();
	float getTimeMilliseconds();
}


#endif