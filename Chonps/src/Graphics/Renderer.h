#ifndef HG_CHONPS_RENDERER_H
#define HG_CHONPS_RENDERER_H

#include "RendererCommand.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Lighting.h"
#include "Core/DataTypes.h"

namespace Chonps
{
	struct RendererStatistics
	{
		uint32_t triangles = 0;
		uint32_t vertices = 0;
		uint32_t indices = 0;
		uint32_t drawCalls = 0;
	};

	struct RendererData
	{
		std::vector<vertex> Vertices;
		RendererStatistics Stats;
	};

	class Renderer
	{
	public:
		static void Init();

		static void Draw(VertexArray* vertexArray);
		static void DrawLine(VertexArray* vertexArray);
		static void BeginScene(Camera& camera, Shader* shader, const char* uniform);
		static void EndScene();

		static RendererStatistics& GetStats();
		static void ResetStats();

	private:
		static RendererData s_Data;
	};

	void renderBeginScene(Camera& camera, Shader* shader, const char* uniform = "camMatrix");
	void renderEndScene();
}

#endif