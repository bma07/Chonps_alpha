#include "cepch.h"
#include "ChonpsGui.h"

#include "Graphics/Renderer.h"

namespace Chonps
{
	static Window* s_CurrentWindow = nullptr;
	static ChonpsGuiContext s_ChonpsGuiContext;
	
	namespace gui
	{
		void SetCurrentWindow(Window* window)
		{
			CHONPS_CORE_ASSERT(window != nullptr, "Window is null");

			window->SetContextCurrent();
			s_CurrentWindow = window;
			s_ChonpsGuiContext.windowWidth = window->GetWidth();
			s_ChonpsGuiContext.windowHeight = window->GetHeight();

			s_ChonpsGuiContext.centerX = s_ChonpsGuiContext.windowWidth / 2;
			s_ChonpsGuiContext.centerY = s_ChonpsGuiContext.windowHeight / 2;
		}

		void SetWindowMode(WindowDrawMode mode)
		{
			s_ChonpsGuiContext.windowDrawMode = mode;

			if (mode == WindowDrawModeGraphing)
			{
				s_ChonpsGuiContext.centerX = s_ChonpsGuiContext.windowWidth / 2;
				s_ChonpsGuiContext.centerY = s_ChonpsGuiContext.windowHeight / 2;
			}
			else if (mode == WindowDrawModeCorner)
			{
				s_ChonpsGuiContext.centerX = 0;
				s_ChonpsGuiContext.centerY = 0;
			}
		}

		bool IsCurrentWindowSet()
		{
			if (s_CurrentWindow == nullptr) 
				return false;
			
			return true;
		}

		void SetShader(Shader* shader)
		{
			s_ChonpsGuiContext.shader = shader;
		}

		void Init()
		{

		}

		void Terminate()
		{
			
		}

		void Begin()
		{
			s_ChonpsGuiContext.shader->Bind();
		}

		void End()
		{
			s_ChonpsGuiContext.shader->Unbind();


		}

		void DrawLine(int x1, int y1, int x2, int y2)
		{
			CHONPS_CORE_ASSERT(s_CurrentWindow != nullptr, "Current Window has not been set!");

			std::vector<vertex> vertices = 
			{
				{ {(float)x1, (float)y1, 0.0f}, {1.0f, 1.0f, 1.0f} },
				{ {(float)x2, (float)y2, 0.0f}, {1.0f, 1.0f, 1.0f} }
			};

			VertexArray* lineVAO = createVertexArray();
			lineVAO->Bind();
			VertexBuffer* lineVBO = createVertexBuffer(vertices);
			lineVAO->LinkVertexBuffer(lineVBO, 0, 3, SDT::Float, sizeof(vertex), (void*)0);
			lineVAO->LinkVertexBuffer(lineVBO, 1, 3, SDT::Float, sizeof(vertex), (void*)(3 * sizeof(float)));
			lineVAO->Unbind();
			lineVBO->Unbind();

			DrawCommand drawCommand{};
			drawCommand.vertexArray = lineVAO;
			s_ChonpsGuiContext.drawList.drawCommands.push_back(drawCommand);

			renderDrawLine(lineVAO);
		}
	}
}

