#ifndef HG_CHONPS_GUI_H
#define HG_CHONPS_GUI_H

#include "Core/Window.h"
#include "Graphics/Shader.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/Font.h"

namespace Chonps
{
	struct ChonpsGuiColorContext
	{
		float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
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

	struct ChonpsGuiColor
	{
		float r, g, b, a;
	};

	struct ChonpsGuiRectData
	{
		ChonpsGuiPos pos;
		ChonpsGuiDimensions dimensions;
		ChonpsGuiColor color;
		BatchRenderer* batchRenderer = nullptr;
	};

	struct ChonpsGuiTriangleData
	{
		ChonpsGuiPos p1, p2, p3;
		ChonpsGuiColor color;
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

	struct ChonpsGuiContext
	{
		int windowWidth, windowHeight;
		int centerX, centerY;
		ChonpsGuiColorContext drawColor;

		Shader* preStencilShader;
		Shader* postStencilShader;
		UniformBuffer* uniformBuffer;
		
		BatchRenderer batchRenderer, stencilRenderer, fontRenderer;

		std::unordered_map<uint32_t, Font> fontLibrary;
	};

	namespace gui
	{
		void SetCurrentWindow(Window* window);
		bool CheckCurrentWindowSet();
		void SetShader(Shader* preStencil, Shader* postStencil, UniformBuffer* uniformBuffer);
		void Init();
		void Terminate();

		void BeginDraw();
		void SubmitDraw();

		void AddFontFromTTF(Font font);
		void AddFontFromTTF(const char* fontFilepath);

		void DrawColor(float r, float g, float b, float a);
		void DrawRect(float x, float y, float width, float height);
		void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
		void DrawPoly(float x, float y, float radius, int nSides);
		void DrawRectRoundCorners(float x, float y, float width, float height, float rounding);

		void DrawRectEx(const ChonpsGuiRectData& rectData);
		void DrawTriangleEx(const ChonpsGuiTriangleData& triangleData);
		void DrawPolyEx(const ChonpsGuiPolyData& polyData);

	}
}

#endif