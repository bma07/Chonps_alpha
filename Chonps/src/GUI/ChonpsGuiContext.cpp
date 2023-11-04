#include "cepch.h"
#include "ChonpsGuiContext.h"

#include "Graphics/Renderer.h"
#include "ChonpsGuiShaders.h"
#include "Core/Input.h"

#include <glm/gtc/type_ptr.hpp>

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

			if (s_WindowInstance != nullptr)
			{
				s_ChonpsGuiContext.windowHostWidth = window->GetWidth();
				s_ChonpsGuiContext.windowHostHeight = window->GetHeight();

				s_ChonpsGuiContext.hostCenterX = s_ChonpsGuiContext.windowHostWidth / 2;
				s_ChonpsGuiContext.hostCenterY = s_ChonpsGuiContext.windowHostHeight / 2;
			}
		}

		Window* GetWindowHost()
		{
			return s_WindowInstance;
		}

		bool CheckCurrentWindowSet()
		{
			if (s_WindowInstance == nullptr)
				return false;

			return true;
		}

		void uploadUniformData(Shader* shader)
		{
			switch (s_ChonpsGuiContext.graphicsContext)
			{
			case GraphicsContext::Vulkan:
			{
				vks::renderPushConstant(shader, ShaderStage::Vertex, sizeof(ChonpsGuiPushConstantRange), 0, &s_ChonpsGuiContext.pushConstant);
				break;
			}
			case GraphicsContext::OpenGL:
			{
				ogls::uploadUniform4mfv(shader->id(), s_ChonpsGuiContext.uniformName, 1, false, glm::value_ptr(s_ChonpsGuiContext.pushConstant.camMatrix));
				break;
			}
			}
		}

		bool CreateGuiContext()
		{
			CHONPS_CORE_ASSERT(!s_GuiInstance, "Attempted to Init gui twice!");

			if (s_GuiInstance)
			{
				CHONPS_CORE_LOG_ERROR(GUI, "Attempting to call gui Init twice even though gui has already been intialized!");
				return false;
			}
			s_GuiInstance = true;

			RendererBackends* backends = getRendererBackends();
			s_ChonpsGuiContext.graphicsContext = getGraphicsContext();
			s_ChonpsGuiContext.drawColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // Set default draw color

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

			s_ChonpsGuiContext.windowRenderer = BatchRenderer(&batchInfo);
			s_ChonpsGuiContext.widgetsRenderer = BatchRenderer(&batchInfo);
			s_ChonpsGuiContext.fontRenderer = BatchRenderer(&batchInfo);

			// Shaders & Pipeline
			PushConstantRange pcr{};
			pcr.size = sizeof(ChonpsGuiPushConstantRange);
			pcr.offset = 0;
			pcr.shaderStage = ShaderStage::Vertex;

			vks::setUsePushConstant(true);
			vks::setPushConstantRange(&pcr, 1);

			DescriptorImageLayoutIncludes layoutInclude = { 0, ImageLayoutOption::TextureImages };

			// PostStencil Pipeline
			PipelineSpecification guiPipelineSpec = getStandardPipelineSpecification();
			guiPipelineSpec.rasterizer.cullMode = CullFaceMode::Disable;
			guiPipelineSpec.depthstencil.enableStencil = true;
			guiPipelineSpec.depthstencil.stencil.compareOp = CompareOperation::LessOrEqual;
			guiPipelineSpec.depthstencil.stencil.failOp = StencilOperation::Keep;
			guiPipelineSpec.depthstencil.stencil.depthFailOp = StencilOperation::Keep;
			guiPipelineSpec.depthstencil.stencil.passOp = StencilOperation::IncrementAndClamp;
			guiPipelineSpec.depthstencil.stencil.reference = 1;
			guiPipelineSpec.depthstencil.stencil.compareMask = 0xff;
			guiPipelineSpec.depthstencil.stencil.writeMask = 0x00;
			guiPipelineSpec.depthstencil.enableDepth = false;

			PipelineLayoutInfo guipipelineLayoutInfo{};
			guipipelineLayoutInfo.bindingDescription = 9 * sizeof(float);
			guipipelineLayoutInfo.vertexLayoutLinkInfo.layoutCount = static_cast<uint32_t>(vertexLayouts.size());
			guipipelineLayoutInfo.vertexLayoutLinkInfo.pLayouts = vertexLayouts.data();
			guipipelineLayoutInfo.bufferLayoutsCount = 0;
			guipipelineLayoutInfo.pBufferLayouts = nullptr;
			guipipelineLayoutInfo.layoutIncludeCount = 1;
			guipipelineLayoutInfo.pLayoutIncludes = &layoutInclude;
			guipipelineLayoutInfo.pipelineSpecification = &guiPipelineSpec;

			// Create Shaders
			std::string guiVertSrc, guiFragSrc, fontVertSrc, fontFragSrc;

			switch (s_ChonpsGuiContext.graphicsContext)
			{
			case GraphicsContext::Vulkan:
			{
				guiVertSrc = guiVert_vkImpl;
				guiFragSrc = guiFrag_vkImpl;
				fontVertSrc = fontVert_vkImpl;
				fontFragSrc = fontFrag_vkImpl;
				break;
			}
			case GraphicsContext::OpenGL:
			{
				guiVertSrc = guiVert_oglImpl;
				guiFragSrc = guiFrag_oglImpl;
				fontVertSrc = fontVert_oglImpl;
				fontFragSrc = fontFrag_oglImpl;
				break;
			}
			default:
			{
				CHONPS_CORE_ASSERT(false, "Gui Shaders not supported under current graphics context!");
				break;
			}
			}

			s_ChonpsGuiContext.guiShader = createShaderSrc(guiVertSrc, guiFragSrc, &guipipelineLayoutInfo);
			s_ChonpsGuiContext.fontShader = createShaderSrc(fontVertSrc, fontFragSrc, &guipipelineLayoutInfo);

			vks::setUsePushConstant(false);
			vks::setPushConstantRange(nullptr, 0);

			loadFontfromTTF("D:/Dev/Chonps/ChonpsEditor/res/Fonts/JetBrainsMono-Regular.ttf", 16, &s_ChonpsGuiContext.defaultFont);
			Texture* texture = createTexture(
				s_ChonpsGuiContext.defaultFont.atlas.width,
				s_ChonpsGuiContext.defaultFont.atlas.height,
				1,
				s_ChonpsGuiContext.defaultFont.atlas.pixels,
				TexType::Diffuse,
				{ TexFilter::Nearest, TexFilter::Nearest }
			);

			// Texture Array
			TextureCreateInfo textures = { texture, 0 };
			s_ChonpsGuiContext.fontTextures = createTextureLayout(&textures, 1, 0);


			// Style
			s_ChonpsGuiContext.styleColors[StyleColor_WindowBg] = { 0.5f, 0.5f, 0.7f, 1.0f };
			s_ChonpsGuiContext.styleColors[StyleColor_WindowTitleBar] = { 0.3f, 0.3f, 0.6f, 1.0f };
			s_ChonpsGuiContext.styleColors[StyleColor_WindowTitleFont] = { 1.0f, 1.0f, 1.0f, 1.0f };
			s_ChonpsGuiContext.styleColors[StyleColor_WindowCloseButton] = { 0.4f, 0.4f, 0.6f, 0.6f };

			return true;
		}

		ChonpsGuiContext* GetGUIContext()
		{
			return &s_ChonpsGuiContext;
		}

		bool SetGuiWindowContext(Window* window)
		{
			SetCurrentWindow(window);
			if (s_WindowInstance == nullptr)
				return false;

			s_ChonpsGuiContext.renderSpace = GuiRenderSpace(0.0f, static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()), 0.0f, -1.0f, 1.0f);
			s_ChonpsGuiContext.renderSpace.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			s_ChonpsGuiContext.renderSpace.SetRotation(0.0f);

			return true;
		}

		bool TerminateGuiContext()
		{
			// Destroy Buffers
			s_ChonpsGuiContext.windowRenderer.Delete();
			s_ChonpsGuiContext.widgetsRenderer.Delete();
			s_ChonpsGuiContext.fontRenderer.Delete();

			s_ChonpsGuiContext.guiShader->Delete();
			s_ChonpsGuiContext.fontShader->Delete();

			s_ChonpsGuiContext.fontTextures->Delete();

			return true;
		}

		void BeginDraw()
		{
			s_ChonpsGuiContext.windowRenderer.DrawBegin();
			s_ChonpsGuiContext.windowHostWidth = s_WindowInstance->GetWidth();
			s_ChonpsGuiContext.windowHostHeight = s_WindowInstance->GetHeight();
			s_ChonpsGuiContext.stencilReference = 0;
		}

		void SubmitDraw()
		{
			s_ChonpsGuiContext.renderSpace.SetDimensions(0.0f, static_cast<float>(s_WindowInstance->GetWidth()), static_cast<float>(s_WindowInstance->GetHeight()), 0.0f, -1.0f, 1.0f);
			s_ChonpsGuiContext.renderSpace.UpdateMatrix();
			s_ChonpsGuiContext.pushConstant.camMatrix = s_ChonpsGuiContext.renderSpace.GetProjectionViewMatrix();
			s_ChonpsGuiContext.pushConstant.unitRange = (20.0f / 48.0f * 8.0f);

			s_ChonpsGuiContext.guiShader->Bind();
			uploadUniformData(s_ChonpsGuiContext.guiShader);
			renderSetStencilReference(s_ChonpsGuiContext.stencilReference);
			renderSetStencilMask(0xff, 0xff);
			
			static bool p_open = true;
			BeginWindow("Window1", &p_open);

			ChonpsGuiRectData rectData{};
			rectData.pos = { 200.0f, 0.0f };
			rectData.dimensions = { 800.0f, 300.0f };
			rectData.color = { 0.5f, 0.2f, 0.6f, 0.8f };
			rectData.texUV = { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} };
			rectData.batchRenderer = &s_ChonpsGuiContext.windowRenderer;
			rectData.rotation = getTimeSeconds() * 20;
			DrawRectExt2(rectData);
			
			s_ChonpsGuiContext.windowRenderer.DrawSubmit();

			/*
			s_ChonpsGuiContext.fontShader->Bind();
			uploadUniformData(s_ChonpsGuiContext.fontShader);
			s_ChonpsGuiContext.fontTextures->Bind(s_ChonpsGuiContext.fontShader);
			s_ChonpsGuiContext.fontRenderer.DrawSubmit();
			RenderText("Hello & Welcome to Chonps Engine Framework! :)", 100.0f, 250.0f, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
			RenderText("Hello & Welcome to Chonps Engine Framework! :)", 400.0f, 250.0f, 1.0f, { 1.0f, 0.0f, 1.0f, 1.0f });
			*/

			s_ChonpsGuiContext.fontShader->Bind();
			uploadUniformData(s_ChonpsGuiContext.fontShader);
			s_ChonpsGuiContext.fontTextures->Bind(s_ChonpsGuiContext.fontShader);
			renderSetStencilReference(++s_ChonpsGuiContext.stencilReference);
			renderSetStencilMask(0xff, 0x00);
			s_ChonpsGuiContext.fontRenderer.DrawSubmit();
		}

		void AddFontFromTTF(Font font)
		{
			s_ChonpsGuiContext.fontLibrary[s_ChonpsGuiContext.fontIDs.take_next()] = font;
		}

		void AddFontFromTTF(const char* fontFilepath, float fontSize)
		{
			Font font;
			if (!loadFontfromTTF(fontFilepath, fontSize, &font))
			{
				CHONPS_CORE_LOG_ERROR(GUI:Font, "Could not load font from filepath: {0}", fontFilepath);
				return;
			}

			s_ChonpsGuiContext.fontLibrary[s_ChonpsGuiContext.fontIDs.take_next()] = std::move(font);
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

			s_ChonpsGuiContext.windowRenderer.PushDrawCommand(drawCmd);
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

			s_ChonpsGuiContext.windowRenderer.PushDrawCommand(drawCmd);
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

			s_ChonpsGuiContext.windowRenderer.PushDrawCommand(drawCmd);
		}

		void DrawRectRoundCorners(float x, float y, float width, float height, float rounding)
		{
			
		}

		void DrawRectExt(const ChonpsGuiRectData& rectData)
		{
			float rectVertices[] =
			{
				//										   pos											  |									 color								   |				 texUVs					 | index
				rectData.pos.x, rectData.pos.y + rectData.dimensions.height,								rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, rectData.texUV.tl.x, rectData.texUV.tl.y, 0.0f,
				rectData.pos.x, rectData.pos.y,																rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, rectData.texUV.bl.x, rectData.texUV.bl.y, 0.0f,
				rectData.pos.x + rectData.dimensions.width, rectData.pos.y,									rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, rectData.texUV.br.x, rectData.texUV.br.y, 0.0f,
				rectData.pos.x + rectData.dimensions.width, rectData.pos.y + rectData.dimensions.height,	rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, rectData.texUV.tr.x, rectData.texUV.tr.y, 0.0f,
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

			rectData.batchRenderer == nullptr ? s_ChonpsGuiContext.windowRenderer.PushDrawCommand(drawCmd) : rectData.batchRenderer->PushDrawCommand(drawCmd);
		}

		void DrawTriangleExt(const ChonpsGuiTriangleData& triangleData)
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

			triangleData.batchRenderer == nullptr ? s_ChonpsGuiContext.windowRenderer.PushDrawCommand(drawCmd) : triangleData.batchRenderer->PushDrawCommand(drawCmd);
		}

		void DrawPolyExt(const ChonpsGuiPolyData& polyData)
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

			polyData.batchRenderer == nullptr ? s_ChonpsGuiContext.windowRenderer.PushDrawCommand(drawCmd) : polyData.batchRenderer->PushDrawCommand(drawCmd);
		}

		void DrawRectExt2(const ChonpsGuiRectData& rectData)
		{
			float rectVertices[] =
			{
				//										   pos											  |									 color								   |				 texUVs					 | index
				rectData.pos.x, rectData.pos.y + rectData.dimensions.height,								rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, rectData.texUV.tl.x, rectData.texUV.tl.y, 0.0f,
				rectData.pos.x, rectData.pos.y,																rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, rectData.texUV.bl.x, rectData.texUV.bl.y, 0.0f,
				rectData.pos.x + rectData.dimensions.width, rectData.pos.y,									rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, rectData.texUV.br.x, rectData.texUV.br.y, 0.0f,
				rectData.pos.x + rectData.dimensions.width, rectData.pos.y + rectData.dimensions.height,	rectData.color.r, rectData.color.g, rectData.color.b, rectData.color.a, rectData.texUV.tr.x, rectData.texUV.tr.y, 0.0f,
			};

			glm::vec2 position[4] =
			{
				{ 0, rectData.dimensions.height },
				{ 0, 0 },
				{ rectData.dimensions.width, 0 },
				{ rectData.dimensions.width, rectData.dimensions.height },
			};


			for (int i = 0; i < 4; i++)
			{
				glm::vec4 newPos = glm::rotate(glm::mat4(1.0f), glm::radians(rectData.rotation), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(position[i].x, position[i].y, 0.0f, 1.0f);
				rectVertices[i * 9] = newPos.x + rectData.pos.x;
				rectVertices[i * 9 + 1] = newPos.y + rectData.pos.y;
			}

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

			rectData.batchRenderer == nullptr ? s_ChonpsGuiContext.windowRenderer.PushDrawCommand(drawCmd) : rectData.batchRenderer->PushDrawCommand(drawCmd);
		}

		void DrawTriangleExt2(const ChonpsGuiTriangleData& triangleData)
		{

		}

		void RenderText(const std::string& text, float x, float y, float scale, ChonpsGuiColor color)
		{
			// iterate through all characters
			for (auto c = text.begin(); c != text.end(); c++)
			{
				FontGlyph ch = s_ChonpsGuiContext.defaultFont.glyphs[*c];

				float xpos = x + ch.bearing.x * scale;
				float ypos = y + (ch.size.y - ch.bearing.y) * scale;

				float w = ch.size.x * scale;
				float h = ch.size.y * scale;

				// Note: height is inverted on host window because of text bearings and placement

				float rectVertices[] =
				{
					//      pos        |			   color			   |     texUVs		 | index
					xpos, ypos,			color.r, color.g, color.b, color.a, ch.uv.x0, ch.uv.y1, 0.0f,
					xpos, ypos - h,		color.r, color.g, color.b, color.a, ch.uv.x0, ch.uv.y0, 0.0f,
					xpos + w, ypos - h, color.r, color.g, color.b, color.a, ch.uv.x1, ch.uv.y0, 0.0f,
					xpos + w, ypos,		color.r, color.g, color.b, color.a, ch.uv.x1, ch.uv.y1, 0.0f,
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

				s_ChonpsGuiContext.fontRenderer.PushDrawCommand(drawCmd);
				
				x += ch.advance * scale;
			}
		}

		void RenderTextExt(const ChonpsGuiTextData& textData)
		{
			float advancex = textData.pos.x;

			BatchRenderer* batchRenderer = nullptr;
			textData.batchRenderer == nullptr ? batchRenderer = &s_ChonpsGuiContext.fontRenderer : batchRenderer = textData.batchRenderer;

			for (const char* c = textData.text; *c != '\0'; c++)
			{
				FontGlyph ch = textData.font->glyphs[*c];

				float xpos = advancex + ch.bearing.x * textData.scale;
				float ypos = textData.pos.y + (ch.size.y - ch.bearing.y) * textData.scale;

				float w = ch.size.x * textData.scale;
				float h = ch.size.y * textData.scale;

				ChonpsGuiColor color = textData.color;

				// Note: height is inverted on host window because of text bearings and placement

				float rectVertices[] =
				{
					//      pos        |			   color			   |     texUVs		 | index
					xpos, ypos,			color.r, color.g, color.b, color.a, ch.uv.x0, ch.uv.y1, 0.0f,
					xpos, ypos - h,		color.r, color.g, color.b, color.a, ch.uv.x0, ch.uv.y0, 0.0f,
					xpos + w, ypos - h, color.r, color.g, color.b, color.a, ch.uv.x1, ch.uv.y0, 0.0f,
					xpos + w, ypos,		color.r, color.g, color.b, color.a, ch.uv.x1, ch.uv.y1, 0.0f,
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

				batchRenderer->PushDrawCommand(drawCmd);

				advancex += ch.advance * textData.scale;
			}
		}

		void PositionRotate(ChonpsGuiPos* vertices, uint32_t count, float degrees)
		{
			glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), degrees, glm::vec3(0.0f, 0.0f, 1.0f));

		}
	}
}

