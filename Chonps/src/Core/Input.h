#ifndef HG_CHONPS_INPUT_H
#define HG_CHONPS_INPUT_H

#include "Window.h"
#include "DataTypes.h"

#include "KeyCodes.h"
#include "MouseButtonCodes.h"

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

		virtual pair<float, float> GetMousePos(Window* window) = 0;
		virtual void GetMousePos(Window* window, float* xpos, float* ypos) = 0;
		virtual float GetMouseX(Window* window) = 0;
		virtual float GetMouseY(Window* window) = 0;

		virtual void SetMousePos(Window* window, float x, float y) = 0;

		virtual pair<int, int> GetWindowPos(Window * window) = 0;
		virtual void GetWindowPos(Window* window, int* xpos, int* ypos) = 0;
		virtual pair<int, int> GetWindowSize(Window* window) = 0;
		virtual void GetWindowSize(Window* window, int* width, int* height) = 0;

		virtual void SetMouseModeHide(Window* window, bool hide) = 0;
		virtual void SetMouseCursor(Window* window, int cursor) = 0;

		virtual float GetTimestep() = 0;
	};

	// Input Functions
	bool keyPressed(Window* window, int keycode);
	bool keyReleased(Window* window, int keycode);
	bool mouseButtonPressed(Window* window, int button);
	bool mouseButtonReleased(Window* window, int button);

	pair<float, float> getMousePos(Window* window);
	void getMousePos(Window* window, float* xpos, float* ypos);
	float getMouseX(Window* window);
	float getMouseY(Window* window);

	void setMousePos(Window* window, float x, float y);

	pair<int, int> getWindowPos(Window* window);
	void getWindowPos(Window* window, int* xpos, int* ypos);
	pair<int, int> getWindowSize(Window* window);
	void getWindowSize(Window* window, int* width, int* height);

	void setMouseModeHide(Window* window, bool hide);
	void setMouseCursor(Window* window, int cursor);

	// Timestep Functions
	float getTimeSeconds();
	float getTimeMilliseconds();
}


#endif