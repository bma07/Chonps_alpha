#ifndef IMGUI_WINDOW_H
#define IMGUI_WINDOW_H

#include "Core/Window.h"

namespace Chonps
{
	void imguiInit(Window* window);

	void imguiNewFrame();

	void imguiNewFrame(Window* window);

	void imguiRender();

	void imguiShutdown();

	void imguiTooltip(const char* tooltip, float delay_time);
}


#endif