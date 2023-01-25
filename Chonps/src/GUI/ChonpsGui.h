#ifndef CHONPS_GUI
#define CHONPS_GUI

#include "Core/Window.h"
#include "Graphics/VertexArray.h"

namespace Chonps
{
	struct ChonpsGuiContext
	{
		int WindowWidth, WindowHeight;
	};

	namespace gui
	{
		void SetCurrentWindow(Window* window);
		bool IsCurrentWindowSet();

		void DrawLine(int x1, int y1, int x2, int y2);

	}
}

#endif