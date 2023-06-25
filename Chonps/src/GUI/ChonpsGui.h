#ifndef CHONPS_GUI
#define CHONPS_GUI

#include "Core/Window.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/DrawList.h"

namespace Chonps
{
	enum WindowDrawMode
	{
		WindowDrawModeGraphing, WindowDrawModeCorner
	};

	struct ChonpsGuiContext
	{
		int windowWidth, windowHeight;
		WindowDrawMode windowDrawMode = WindowDrawModeGraphing;
		int centerX, centerY;
		Shader* shader;

		DrawList drawList;
	};

	namespace gui
	{
		void SetCurrentWindow(Window* window);
		void SetWindowMode(WindowDrawMode mode);
		bool IsCurrentWindowSet();
		void SetShader(Shader* shader);
		void Init();
		void Terminate();

		void Begin();
		void End();

		void DrawLine(int x1, int y1, int x2, int y2);

	}
}

#endif