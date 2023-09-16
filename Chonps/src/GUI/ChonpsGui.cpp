#include "cepch.h"
#include "ChonpsGui.h"

#include "Graphics/Renderer.h"

namespace Chonps
{
	static Window* s_WindowInstance = nullptr;
	static ChonpsGuiContext s_ChonpsGuiContext;
	static bool s_GuiInstance = false;

	namespace gui
	{	
		void SetCurrentWindow(Window* window)
		{
			CHONPS_CORE_ASSERT(window != nullptr, "Window is null");

			window->SetContextCurrent();
			s_WindowInstance = window;
			s_ChonpsGuiContext.windowWidth = window->GetWidth();
			s_ChonpsGuiContext.windowHeight = window->GetHeight();

			s_ChonpsGuiContext.centerX = s_ChonpsGuiContext.windowWidth / 2;
			s_ChonpsGuiContext.centerY = s_ChonpsGuiContext.windowHeight / 2;
		}

		bool CheckCurrentWindowSet()
		{
			if (s_WindowInstance == nullptr) 
				return false;
			
			return true;
		}

		void SetShader(Shader* preStencil, Shader* postStencil, UniformBuffer* uniformBuffer)
		{
			s_ChonpsGuiContext.preStencilShader = preStencil;
			s_ChonpsGuiContext.postStencilShader = postStencil;
			s_ChonpsGuiContext.uniformBuffer = uniformBuffer;
		}

		void Init()
		{
			CHONPS_CORE_ASSERT(!s_GuiInstance, "Attempted to Init gui twice!");
			
			if (s_GuiInstance)
			{
				CHONPS_CORE_LOG_ERROR(GUI, "Attempting to call gui Init twice even though gui has already been intialized!");
				return;
			}
			s_GuiInstance = true;

			RendererBackends* backends = getRendererBackends();

			std::vector<VertexLayout> vertexLayouts =
			{
				{ 0, 2, SDT::Float, 9 * sizeof(float), (void*)(0) },
				{ 1, 4, SDT::Float, 9 * sizeof(float), (void*)(2 * sizeof(float)) },
				{ 2, 2, SDT::Float, 9 * sizeof(float), (void*)(6 * sizeof(float)) },
				{ 3, 1, SDT::Float, 9 * sizeof(float), (void*)(8 * sizeof(float)) },
			};

			BatchCreateInfo batchInfo{};
			batchInfo.pLayouts = vertexLayouts.data();
			batchInfo.layoutCount = static_cast<uint32_t>(vertexLayouts.size());
			batchInfo.vertexBufferSize = static_cast<uint32_t>(9 * sizeof(float) * backends->maxGuiVerticesPerObjectHint * backends->maxGuiDrawObjects);
			batchInfo.indexBufferSize = static_cast<uint32_t>(sizeof(uint32_t) * backends->maxGuiIndicesPerObjectHint * backends->maxGuiDrawObjects);

			s_ChonpsGuiContext.batchRenderer = BatchRenderer(&batchInfo);
			s_ChonpsGuiContext.stencilRenderer = BatchRenderer(&batchInfo);
			s_ChonpsGuiContext.fontRenderer = BatchRenderer(&batchInfo);
		}

		void Terminate()
		{
			// Destroy Buffers
			s_ChonpsGuiContext.batchRenderer.Delete();
			s_ChonpsGuiContext.stencilRenderer.Delete();
			s_ChonpsGuiContext.fontRenderer.Delete();
		}

		void BeginDraw()
		{
			s_ChonpsGuiContext.batchRenderer.DrawBegin();
		}

		void SubmitDraw()
		{
			s_ChonpsGuiContext.preStencilShader->Bind();
			renderSetStencilReference(1);
			renderSetStencilMask(0xff, 0xff);
			s_ChonpsGuiContext.batchRenderer.DrawSubmit();


			ChonpsGuiRectData rectData{};
			rectData.pos = { 200.0f, 400.0f };
			rectData.dimensions = { 800.0f, 300.0f };
			rectData.color = { 0.5f, 0.2f, 0.6f, 0.8f };
			rectData.batchRenderer = &s_ChonpsGuiContext.stencilRenderer;

			DrawRectEx(rectData);

			s_ChonpsGuiContext.postStencilShader->Bind();
			renderSetStencilReference(1);
			renderSetStencilMask(0xff, 0xff);
			s_ChonpsGuiContext.stencilRenderer.DrawSubmit();


			rectData.pos = { 240.0f, 90.0f };
			rectData.dimensions = { 130.0f, 700.0f };
			rectData.color = { 0.8f, 0.8f, 0.8f, 0.8f };
			rectData.batchRenderer = &s_ChonpsGuiContext.fontRenderer;

			DrawRectEx(rectData);

			renderSetStencilReference(2);
			renderSetStencilMask(0xff, 0xff);
			s_ChonpsGuiContext.fontRenderer.DrawSubmit();
		}

		void AddFontFromTTF(Font font)
		{
			s_ChonpsGuiContext.fontLibrary[font.id()] = font;
		}

		void AddFontFromTTF(const char* fontFilepath)
		{
			Font font = Font(fontFilepath);
			s_ChonpsGuiContext.fontLibrary[font.id()] = std::move(font);
		}

		void DrawColor(float r, float g, float b, float a)
		{
			s_ChonpsGuiContext.drawColor = { r, g, b, a };
		}

		void DrawRect(float x, float y, float width, float height)
		{
			float r = s_ChonpsGuiContext.drawColor.r;
			float g = s_ChonpsGuiContext.drawColor.g;
			float b = s_ChonpsGuiContext.drawColor.b;
			float a = s_ChonpsGuiContext.drawColor.a;

			float rectVertices[] =
			{
				//         pos        |   color   |  texUVs   | index
				x, y + height,		   r, g, b, a, 0.0f, 1.0f, 0.0f,
				x, y,				   r, g, b, a, 0.0f, 0.0f, 0.0f,
				x + width, y,		   r, g, b, a, 1.0f, 0.0f, 0.0f,
				x + width, y + height, r, g, b, a, 1.0f, 1.0f, 0.0f,
			};

			uint32_t rectIndices[] =
			{
				0, 1, 2,
				0, 2, 3
			};

			DrawCommand drawCmd{};
			drawCmd.pVertices = rectVertices;
			drawCmd.vertexStride = 9;
			drawCmd.vertexCount = 4;
			drawCmd.pIndices = rectIndices;
			drawCmd.indicesCount = 6;

			s_ChonpsGuiContext.batchRenderer.PushDrawCommand(drawCmd);
		}

		void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
		{
			float r = s_ChonpsGuiContext.drawColor.r;
			float g = s_ChonpsGuiContext.drawColor.g;
			float b = s_ChonpsGuiContext.drawColor.b;
			float a = s_ChonpsGuiContext.drawColor.a;

			float triangleVertices[] =
			{
				// pos |   color   |  texUVs   | index
				x1, y1, r, g, b, a, 0.0f, 0.0f, 0.0f,
				x2, y2, r, g, b, a, 1.0f, 0.0f, 0.0f,
				x3, y3, r, g, b, a, 0.5f, 1.0f, 0.0f,

			};

			uint32_t triangleIndices[] =
			{
				0, 1, 2
			};

			DrawCommand drawCmd{};
			drawCmd.pVertices = triangleVertices;
			drawCmd.vertexStride = 9;
			drawCmd.vertexCount = 3;
			drawCmd.pIndices = triangleIndices;
			drawCmd.indicesCount = 3;

			s_ChonpsGuiContext.batchRenderer.PushDrawCommand(drawCmd);
		}

		void DrawPoly(float x, float y, float radius, int nSides)
		{
			float r = s_ChonpsGuiContext.drawColor.r;
			float g = s_ChonpsGuiContext.drawColor.g;
			float b = s_ChonpsGuiContext.drawColor.b;
			float a = s_ChonpsGuiContext.drawColor.a;

			std::vector<float> vertices = { x, y, r, g, b, a, 0.5f, 0.5f, 0.0f};
			std::vector<uint32_t> indices;

			float doublePi = static_cast<float>(2 * CHONPS_PI);
			float angle = doublePi / nSides;

			for (int i = 0; i < nSides; i++)
			{
				float circlex = std::cos(i * angle);
				float circley = std::sin(i * angle);

				float posx = x + (radius * circlex);
				float posy = y + (radius * circley);

				vertices.push_back(posx);
				vertices.push_back(posy);
				vertices.push_back(r);
				vertices.push_back(g);
				vertices.push_back(b);
				vertices.push_back(a);
				vertices.push_back(circlex);
				vertices.push_back(circley);
				vertices.push_back(0.0f);

				indices.push_back(0);
				indices.push_back(i + 1);
				indices.push_back(i + 2);
			}

			indices.pop_back();
			indices.push_back(indices[1]);

			DrawCommand drawCmd{};
			drawCmd.pVertices = vertices.data();
			drawCmd.vertexStride = 9;
			drawCmd.vertexCount = nSides + 1;
			drawCmd.pIndices = indices.data();
			drawCmd.indicesCount = static_cast<uint32_t>(indices.size());

			s_ChonpsGuiContext.batchRenderer.PushDrawCommand(drawCmd);
		}

		void DrawRectRoundCorners(float x, float y, float width, float height, float rounding)
		{
			
		}

		void DrawRectEx(const ChonpsGuiRectData& rectData)
		{
			float rectVertices[] =
			{
				//										   pos											  |									 color								   |  texUVs   | index
				rectData.pos.x, rectData.pos.y + rectData.dimensions.height,								rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, 0.0f, 1.0f, 0.0f,
				rectData.pos.x, rectData.pos.y,																rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, 0.0f, 0.0f, 0.0f,
				rectData.pos.x + rectData.dimensions.width, rectData.pos.y,									rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, 1.0f, 0.0f, 0.0f,
				rectData.pos.x + rectData.dimensions.width, rectData.pos.y + rectData.dimensions.height,	rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, 1.0f, 1.0f, 0.0f,
			};

			uint32_t rectIndices[] =
			{
				0, 1, 2,
				0, 2, 3
			};

			DrawCommand drawCmd{};
			drawCmd.pVertices = rectVertices;
			drawCmd.vertexStride = 9;
			drawCmd.vertexCount = 4;
			drawCmd.pIndices = rectIndices;
			drawCmd.indicesCount = 6;

			rectData.batchRenderer == nullptr ? s_ChonpsGuiContext.batchRenderer.PushDrawCommand(drawCmd) : rectData.batchRenderer->PushDrawCommand(drawCmd);
		}

		void DrawTriangleEx(const ChonpsGuiTriangleData& triangleData)
		{
			float triangleVertices[] =
			{
				//				 pos				 |										   color										 |  texUVs   | index
				triangleData.p1.x, triangleData.p1.y, triangleData.color.r, triangleData.color.g, triangleData.color.b, triangleData.color.a, 0.0f, 0.0f, 0.0f,
				triangleData.p2.x, triangleData.p2.y, triangleData.color.r, triangleData.color.g, triangleData.color.b, triangleData.color.a, 1.0f, 0.0f, 0.0f,
				triangleData.p3.x, triangleData.p3.y, triangleData.color.r, triangleData.color.g, triangleData.color.b, triangleData.color.a, 0.5f, 1.0f, 0.0f,

			};

			uint32_t triangleIndices[] =
			{
				0, 1, 2
			};

			DrawCommand drawCmd{};
			drawCmd.pVertices = triangleVertices;
			drawCmd.vertexStride = 9;
			drawCmd.vertexCount = 3;
			drawCmd.pIndices = triangleIndices;
			drawCmd.indicesCount = 3;

			triangleData.batchRenderer == nullptr ? s_ChonpsGuiContext.batchRenderer.PushDrawCommand(drawCmd) : triangleData.batchRenderer->PushDrawCommand(drawCmd);
		}

		void DrawPolyEx(const ChonpsGuiPolyData& polyData)
		{
			std::vector<float> vertices =
			{
				polyData.pos.x, polyData.pos.y,
				polyData.color.r, polyData.color.g, polyData.color.b, polyData.color.a,
				0.5f, 0.5f, 0.0f
			};
			std::vector<uint32_t> indices;

			float doublePi = static_cast<float>(2 * CHONPS_PI);
			float angle = doublePi / polyData.nSides;

			for (int i = 0; i < polyData.nSides; i++)
			{
				float circlex = std::cos(i * angle);
				float circley = std::sin(i * angle);

				float posx = polyData.pos.x + (polyData.radius * circlex);
				float posy = polyData.pos.y + (polyData.radius * circley);

				vertices.push_back(posx);
				vertices.push_back(posy);
				vertices.push_back(polyData.color.r);
				vertices.push_back(polyData.color.g);
				vertices.push_back(polyData.color.b);
				vertices.push_back(polyData.color.a);
				vertices.push_back(circlex);
				vertices.push_back(circley);
				vertices.push_back(0.0f);

				indices.push_back(0);
				indices.push_back(i + 1);
				indices.push_back(i + 2);
			}

			indices.pop_back();
			indices.push_back(indices[1]);

			DrawCommand drawCmd{};
			drawCmd.pVertices = vertices.data();
			drawCmd.vertexStride = 9;
			drawCmd.vertexCount = polyData.nSides + 1;
			drawCmd.pIndices = indices.data();
			drawCmd.indicesCount = static_cast<uint32_t>(indices.size());

			polyData.batchRenderer == nullptr ? s_ChonpsGuiContext.batchRenderer.PushDrawCommand(drawCmd) : polyData.batchRenderer->PushDrawCommand(drawCmd);
		}
	}
}

