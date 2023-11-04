#include "cepch.h"
#include "ChonpsGuiContext.h"

#include "Core/Input.h"

namespace Chonps
{
	bool withinRectArea(float x, float y, float rectx, float recty, float xwidth, float yheight)
	{
		return (x > rectx && x < xwidth && y > recty && y < yheight);
	}

	void calculateWindowInput(ChonpsGuiContext* context, ChonpsGuiWindowData* winData, bool* p_open)
	{
		// Get Host Window and mouse position
		Window* windowHost = gui::GetWindowHost();
		auto [mx, my] = getMousePos(windowHost);

		static bool firstClick = true, // if user clicks, false after first click, true after mouse released
			withinWindow = false,
			nearLeftEdge = false,
			nearRightEdge = false,
			nearTopEdge = false,
			nearBottomEdge = false;
		static float mousex = 0.0f, mousey = 0.0f, winx = 0.0f, winy = 0.0f, winw = 0.0f, winh = 0.0f, widthx = 0.0f, heighty = 0.0f;
		static int mouseCursor = ChonpsMouseCursor_Arrow;
		bool mousePressesd = mouseButtonPressed(windowHost, ChonpsMouseButton_1);

		// If mouse cursor is near window edges
		float dr = context->windowDragRange;
		bool withinLeftEdge = within(mx, winData->x, dr, 0.0f) && my > winData->y - dr && my < winData->y + winData->height + dr;
		bool withinRightEdge = within(mx, winData->x + winData->width, 0.0f, dr) && my > winData->y - dr && my < winData->y + winData->height + dr;
		bool withinBottomEdge = within(my, winData->y, dr, 0.0f) && mx > winData->x - dr && mx < winData->x + winData->width + dr;
		bool withinTopEdge = within(my, winData->y + winData->height, 0.0f, dr) && mx > winData->x - dr && mx < winData->x + winData->width + dr;

		if (p_open != nullptr)
		{
			ChonpsGuiPos closePos = { winData->x + winData->width - winData->barHeight * 0.5f, winData->y + winData->barHeight * 0.5f };
			if (firstClick && within(mx, closePos.x, winData->barHeight * 0.25f) && within(my, closePos.y, winData->barHeight * 0.25f) && mouseButtonPressed(windowHost, ChonpsMouseButton_1))
			{
				*p_open = false;
			}
		}

		if (mousePressesd && firstClick) // Initialize mouse cursor and window data upon first click to calculate window move and resize offset
		{
			mousex = mx;
			mousey = my;
			winx = winData->x;
			winy = winData->y;
			winw = winData->width;
			winh = winData->height;
			widthx = winData->x + winData->width; // Use only for resizeing window from left edge
			heighty = winData->y + winData->height; // Use only for resizeing window from bottom edge
			firstClick = false;
			withinWindow = withinRectArea(
				mx,
				my,
				winData->x,
				winData->y,
				winData->x + winData->width,
				winData->y + winData->barHeight
			);

			nearLeftEdge = withinLeftEdge;
			nearRightEdge = withinRightEdge;
			nearBottomEdge = withinBottomEdge;
			nearTopEdge = withinTopEdge;

			// Display correct cursor direction depending on edge of window
			if ((withinLeftEdge && withinBottomEdge) || (withinRightEdge && withinTopEdge))
				mouseCursor = ChonpsMouseCursor_ResizeNWSE;
			else if ((withinLeftEdge && withinTopEdge) || (withinRightEdge && withinBottomEdge))
				mouseCursor = ChonpsMouseCursor_ResizeNESW;
			else if (withinBottomEdge || withinTopEdge)
				mouseCursor = ChonpsMouseCursor_ResizeNS;
			else if (withinLeftEdge || withinRightEdge)
				mouseCursor = ChonpsMouseCursor_ResizeEW;
			else
				mouseCursor = ChonpsMouseCursor_Arrow;
		}
		else if (mouseButtonReleased(windowHost, ChonpsMouseButton_1))
		{
			firstClick = true;
		}

		if (firstClick) // Display correct cursor direction depending on edge of window
		{
			if ((withinLeftEdge && withinBottomEdge) || (withinRightEdge && withinTopEdge))
				setMouseCursor(windowHost, ChonpsMouseCursor_ResizeNWSE);
			else if ((withinLeftEdge && withinTopEdge) || (withinRightEdge && withinBottomEdge))
				setMouseCursor(windowHost, ChonpsMouseCursor_ResizeNESW);
			else if (withinBottomEdge || withinTopEdge)
				setMouseCursor(windowHost, ChonpsMouseCursor_ResizeNS);
			else if (withinLeftEdge || withinRightEdge)
				setMouseCursor(windowHost, ChonpsMouseCursor_ResizeEW);
			else
				setMouseCursor(windowHost, ChonpsMouseCursor_Arrow);
		}
		else
			setMouseCursor(windowHost, mouseCursor);

		if (mousePressesd && nearLeftEdge) // left edge
		{
			float xoffset = mx - mousex;
			winData->x = winData->width > winData->minWidth ? winx + xoffset : widthx - winData->width;
			winData->width = winw - xoffset;
		}
		else if (mousePressesd && nearRightEdge) // right edge
		{
			float xoffset = mx - mousex;
			winData->width = winw + xoffset;
		}
		if (mousePressesd && nearBottomEdge) // bottom edge
		{
			float yoffset = my - mousey;
			winData->y = winData->height > winData->minHeight ? winy + yoffset : heighty - winData->height;
			winData->height = winh - yoffset;
		}
		else if (mousePressesd && nearTopEdge) // top edge
		{
			float yoffset = my - mousey;
			winData->height = winh + yoffset;
		}
		else if ((mousePressesd && withinWindow)) // within window bar; move window
		{
			float xoffset = mx - mousex;
			float yoffset = my - mousey;

			winData->x = winx + xoffset;
			winData->y = winy + yoffset;
		}

		winData->width = winData->width <= winData->minWidth ? winData->minWidth : winData->width;
		winData->height = winData->height <= winData->minHeight ? winData->minHeight : winData->height;
	}

	ChonpsGuiWindow::ChonpsGuiWindow()
	{
		ChonpsGuiContext* context = gui::GetGUIContext();

		winData.x = 0.0f;
		winData.y = 0.0f;
		winData.width = 0.0f;
		winData.height = 0.0f;
		winData.minWidth = 0.0f;
		winData.minHeight = 0.0f;
		winData.bgColor = context->styleColors[StyleColor_WindowBg];
		winData.titleBarColor = context->styleColors[StyleColor_WindowTitleBar];
		winData.titleFontColor = context->styleColors[StyleColor_WindowTitleFont];
		winData.titleFont = &context->defaultFont;
		winData.minBarHeight = context->windowMinBarHeight;
		winData.barPaddingx = context->windowBarPaddingX;
		winData.barPaddingy = context->windowBarPaddingY;
		winData.barHeight = context->defaultFont.fontSize > winData.minBarHeight ? context->defaultFont.fontSize + 2 * winData.barPaddingy : winData.minBarHeight + 2 * winData.barPaddingy;
	}

	ChonpsGuiWindow::ChonpsGuiWindow(ChonpsGuiWindowData data)
	{
		winData = data;
	}

	namespace gui
	{
		void BeginWindow(const char* title, bool* p_open)
		{
			ChonpsGuiContext* context = gui::GetGUIContext();

			if (context->windows.find(title) == context->windows.end())
				context->windows[title] = std::move(CreateGuiWindow(title));
			
			if (p_open != nullptr && !(*p_open))
				return;

			ChonpsGuiWindow* window = &context->windows[title];
			ChonpsGuiWindowData* winData = &window->winData;
			
			calculateWindowInput(context, winData, p_open);

			DrawWindow(context, window);
		}

		void EndWindow()
		{

		}

		ChonpsGuiWindow CreateGuiWindow(const char* title)
		{
			ChonpsGuiContext* context = gui::GetGUIContext();

			ChonpsGuiWindow window{};
			window.winData.title = title;
			window.winData.width = 400.0f;
			window.winData.height = 350.0f;
			window.winData.minWidth = 100.0f;
			window.winData.minHeight = 50.0f;

			return window;
		}

		void DrawWindow(ChonpsGuiContext* context, ChonpsGuiWindow* window)
		{
			ChonpsGuiWindowData winData = window->winData;
			ChonpsGuiRectData rectData{};

			// draw window area
			rectData.pos = { winData.x, winData.y };
			rectData.dimensions = { winData.width, winData.height };
			rectData.color = winData.bgColor;
			rectData.texUV = { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} };
			rectData.batchRenderer = &context->windowRenderer;
			DrawRectExt(rectData);

			// Draw window title bar
			rectData.dimensions.height = winData.barHeight;
			rectData.color = winData.titleBarColor;
			DrawRectExt(rectData);

			// Window title
			ChonpsGuiTextData textData{};
			textData.text = winData.title;
			textData.pos = { winData.x + winData.barPaddingx, winData.y + winData.barHeight - winData.barPaddingy };
			textData.scale = 1.0f;
			textData.font = &context->defaultFont;
			textData.color = winData.titleFontColor;
			textData.batchRenderer = &context->fontRenderer;
			RenderTextExt(textData);

			ChonpsGuiPolyData polyData{};
			polyData.pos = { winData.x + winData.width - winData.barHeight * 0.5f, winData.y + winData.barHeight * 0.5f };
			polyData.radius = winData.barHeight * 0.25f;
			polyData.nSides = 16;
			polyData.color = context->styleColors[StyleColor_WindowCloseButton];
			polyData.batchRenderer = &context->windowRenderer;
			DrawPolyExt(polyData);
		}
	}
}