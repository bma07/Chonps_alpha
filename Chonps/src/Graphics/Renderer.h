#ifndef RENDERER_H
#define RENDERER_H

#include "RendererAPI.h"
#include "VAO.h"
#include "Camera.h"

namespace Chonps
{
	class Renderer
	{
	public:
		static void RenderInit();

		static void Draw(const size_t& count);
		static void Draw(VAO* VAO);

		static void BeginScene(Camera camera, Shader* shader, const char* uniform = "camMatrix");
		static void EndScene();

		static void Clear();
		static void ClearColor(const float r, const float g, const float b, const float w = 0.0f);

		static RendererAPI* GetRendererAPI();
		static void SetRendererAPI();
	};

	void renderInit();

	void renderDraw(const size_t& count);
	void renderDraw(VAO* VAO);

	void renderBeginScene(Camera camera, Shader* shader, const char* uniform = "camMatrix");
	void renderEndScene();

	void renderClear();
	void renderClearColor(const float r, const float g, const float b, const float w = 0.0f);

	void setRendererAPI();
	RendererAPI* getRendererAPI();
}

#endif