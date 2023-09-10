#ifndef HG_CHONPS_RENDERER_2D_H
#define HG_CHONPS_RENDERER_2D_H


namespace Chonps
{
	class Renderer2D
	{
	public:
		static void ClearColor(float r, float g, float b, float a);
		static void DrawColor(float r, float g, float b, float a);
		static void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
		static void DrawQuad(float x, float y, float width, float height);
		static void DrawPoly(float x, float y, float radius, float nSides);
	};
}


#endif