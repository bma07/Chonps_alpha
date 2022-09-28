#ifndef IMGUI_WINDOW_H
#define IMGUI_WINDOW_H

#include "Window.h"

namespace Chonps
{
	void ImguiInit(Window* window);

	void ImguiNewFrame();

	void ImguiNewFrame(Window* window);

	void ImguiRender();

	void ImguiShutdown();

	void ImguiDraw();
}


#endif