#ifndef INPUT_H
#define INPUT_H

#include "Core.h"
#include "Window.h"
#include "DataTypes.h"

namespace Chonps
{
	bool keyPressed(Window* window, int keycode);
	bool mouseButtonPressed(Window* window, int button);

	vec2f getMousePos(Window* window);
	void getMousePos(Window* window, float* xpos, float* ypos);
	float getMouseX(Window* window);
	float getMouseY(Window* window);

	vec2i getWindowPos(Window* window);
	void getWindowPos(Window* window, int* xpos, int* ypos);
	vec2i getWindowSize(Window* window);
	void getWindowSize(Window* window, int* width, int* height);
}


#endif