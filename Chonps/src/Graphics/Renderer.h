#ifndef CHONPS_RENDERER_H
#define CHONPS_RENDERER_H

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Lighting.h"

namespace Chonps
{
	class Renderer
	{
	public:
		static void RenderInit();

		static void Draw(const size_t& count);
		static void Draw(VertexArray* VertexArray);

		static void BeginScene(Camera camera, Shader* shader, const char* uniform = "camMatrix");
		static void EndScene();

		static void Clear();
		static void ClearColor(const float r, const float g, const float b, const float w = 0.0f);

		static void FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height);

		static void GammaCorrection(bool correct);

		static RendererAPI* GetRendererAPI();
		static void SetRendererAPI();
	};

	void renderInit();

	void renderDraw(const size_t& count);
	void renderDraw(VertexArray* VertexArray);

	void renderBeginScene(Camera camera, Shader* shader, const char* uniform = "camMatrix");
	void renderEndScene();

	void renderClear();
	void renderClearColor(const float r, const float g, const float b, const float w = 0.0f);

	void renderFrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height);

	void renderGammaCorrection(bool correct);

	void setRendererAPI();
	RendererAPI* getRendererAPI();
}

#endif