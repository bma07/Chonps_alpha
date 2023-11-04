#ifndef HG_CHONPS_GUI_CONTEXT_H
#define HG_CHONPS_GUI_CONTEXT_H

#include "Core/Window.h"
#include "Graphics/RendererAPI.h"
#include "Graphics/Shader.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/Font.h"
#include "Graphics/OrthographicCamera.h"

namespace Chonps
{
	struct ChonpsGuiPushConstantRange;
	struct ChonpsGuiVertexData;
	struct ChonpsGuiPos;
	struct ChonpsGuiDimensions;
	struct ChonpsGuiUVCoords;
	struct ChonpsGuiColor;
	struct ChonpsGuiRectData;
	struct ChonpsGuiTriangleData;
	struct ChonpsGuiPolyData;
	struct ChonpsGuiContext;
	using GuiRenderSpace = Chonps::OrthographicCamera;
	struct ChonpsGuiWindowData;
	class ChonpsGuiWindow;

	struct ChonpsGuiPushConstantRange
	{
		glm::mat4 camMatrix;
		float unitRange;
	};

	struct ChonpsGuiVertexData
	{
		float x, y;
		float r, g, b, a;
		float uvx, uvy;
		float texIndex;
	};

	struct ChonpsGuiPos
	{
		float x, y;
	};

	struct ChonpsGuiDimensions
	{
		float width, height;
	};

	struct ChonpsGuiUVCoords
	{
		ChonpsGuiPos bl, br, tl, tr;
	};

	struct ChonpsGuiColor
	{
		float r, g, b, a;
	};

	struct ChonpsGuiRectData
	{
		ChonpsGuiPos pos;
		ChonpsGuiDimensions dimensions;
		ChonpsGuiColor color;
		ChonpsGuiUVCoords texUV;
		float rotation = 0.0f; // rotate in degrees
		BatchRenderer* batchRenderer = nullptr;
	};

	struct ChonpsGuiTriangleData
	{
		ChonpsGuiPos p1, p2, p3;
		ChonpsGuiColor color;
		float rotation = 0.0f; // rotate in degrees
		BatchRenderer* batchRenderer = nullptr;
	};

	struct ChonpsGuiPolyData
	{
		ChonpsGuiPos pos;
		ChonpsGuiColor color;
		float radius;
		int nSides;
		BatchRenderer* batchRenderer = nullptr;
	};

	struct ChonpsGuiTextData
	{
		const char* text;
		ChonpsGuiPos pos;
		float scale;
		ChonpsGuiColor color;
		Font* font;
		BatchRenderer* batchRenderer = nullptr;
	};

	enum ChonpsGuiStyleColors
	{
		StyleColor_WindowBg				= 0,
		StyleColor_WindowTitleBar		= 1,
		StyleColor_WindowTitleFont		= 2,
		StyleColor_WindowCloseButton	= 3,

		StyleColor_MaxStyleColors		= 4
	};
	
	struct ChonpsGuiContext
	{
		// Window
		int									windowHostWidth, windowHostHeight;
		int									hostCenterX, hostCenterY;

		// Rendering
		GraphicsContext						graphicsContext;
		ChonpsGuiColor						drawColor;
		GuiRenderSpace						renderSpace;
		const char*							uniformName = "u_CamMatrix";
		Shader								*guiShader, *fontShader;
		ChonpsGuiPushConstantRange			pushConstant;
		BatchRenderer						windowRenderer, widgetsRenderer, fontRenderer;
		uint32_t							stencilReference = 0;

		// Fonts
		TextureLayout*							fontTextures;
		std::unordered_map<uint32_t, Font>		fontLibrary;
		Font									defaultFont;
		QueueChain<uint32_t>					fontIDs;

		// Style
		ChonpsGuiColor						styleColors[StyleColor_MaxStyleColors];

		// Gui Window
		std::unordered_map<const char*, ChonpsGuiWindow>	windows;
		float												windowDragRange = 5.0f; // Range for clicking on window edge for resizing
		float												windowMinBarHeight = 12.0f;
		float												windowBarPaddingX = 2.0f;
		float												windowBarPaddingY = 2.0f;
	};

	// Gui Window
	struct ChonpsGuiWindowData
	{
		const char* title;
		float x, y, width, height;
		float minWidth, minHeight;
		Font* titleFont;
		float barHeight, minBarHeight;
		float barPaddingx, barPaddingy;

		ChonpsGuiColor bgColor;
		ChonpsGuiColor titleBarColor;
		ChonpsGuiColor titleFontColor;
	};

	class ChonpsGuiWindow
	{
	public:
		ChonpsGuiWindow();
		ChonpsGuiWindow(ChonpsGuiWindowData data);

		void SetPos(float x, float y) { this->winData.x = x; this->winData.y = y; }
		void SetDimensions(float width, float height) { this->winData.width = width; this->winData.height = height; }

		ChonpsGuiWindowData winData;
	};

	namespace gui
	{
		void SetCurrentWindow(Window* window);
		Window* GetWindowHost();
		bool CheckCurrentWindowSet();
		bool CreateGuiContext();
		ChonpsGuiContext* GetGUIContext();
		bool SetGuiWindowContext(Window* window);
		bool TerminateGuiContext();

		void BeginDraw();
		void SubmitDraw();

		void AddFontFromTTF(Font font);
		void AddFontFromTTF(const char* fontFilepath, float fontSize);

		void DrawColor(float r, float g, float b, float a);
		void DrawRect(float x, float y, float width, float height);
		void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
		void DrawPoly(float x, float y, float radius, int nSides);
		void DrawRectRoundCorners(float x, float y, float width, float height, float rounding);

		void DrawRectExt(const ChonpsGuiRectData& rectData);
		void DrawTriangleExt(const ChonpsGuiTriangleData& triangleData);
		void DrawPolyExt(const ChonpsGuiPolyData& polyData);

		void DrawRectExt2(const ChonpsGuiRectData& rectData);
		void DrawTriangleExt2(const ChonpsGuiTriangleData& triangleData);
		
		void RenderText(const std::string& text, float x, float y, float scale, ChonpsGuiColor color);
		void RenderTextExt(const ChonpsGuiTextData& textData);

		void PositionRotate(ChonpsGuiPos* vertices, uint32_t count, float degrees);

		// Gui Window
		void BeginWindow(const char* title, bool* p_open = nullptr);
		void EndWindow();
		ChonpsGuiWindow CreateGuiWindow(const char* title);
		void DrawWindow(ChonpsGuiContext* context, ChonpsGuiWindow* window);
	}
}

#endif