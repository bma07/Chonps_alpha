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
			s_ChonpsGuiContext.WindowWidth = window->GetWidth();
			s_ChonpsGuiContext.WindowHeight = window->GetHeight();
		}

		bool IsCurrentWindowSet()
		{
			if (s_CurrentWindow == nullptr) return false;
			
			return true;
		}

		void DrawLine(int x1, int y1, int x2, int y2)
		{
			CHONPS_CORE_ASSERT(s_CurrentWindow != nullptr, "Current Window has not been set!");

			float vertices[12] =
			{
				(float)x1, (float)y1, 0.0f, 1.0f, 1.0f, 1.0f,
				(float)x2, (float)y2, 0.0f, 1.0f, 1.0f, 1.0f
			};

			std::shared_ptr<VertexArray> lineVAO = createVertexArray();
			lineVAO->Bind();
			std::shared_ptr<VertexBuffer> lineVBO = createVertexBuffer(vertices, sizeof(vertices));
			lineVAO->LinkVertexBuffer(&(*lineVBO), 0, 3, SDT::Float, 6 * sizeof(float), (void*)0);
			lineVAO->LinkVertexBuffer(&(*lineVBO), 1, 3, SDT::Float, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			lineVAO->Unbind();
			lineVBO->Unbind();

			lineVAO->Bind();
			renderDrawLine(&(*lineVAO));
			lineVAO->Unbind();

			lineVAO->Delete();
			lineVBO->Delete();
		}
	}
}

