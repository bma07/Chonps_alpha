#ifndef HG_CHONPS_IMGUI_WINDOW_H
#define HG_CHONPS_IMGUI_WINDOW_H

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