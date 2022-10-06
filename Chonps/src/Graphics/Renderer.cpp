#include "cepch.h"
#include "Renderer.h"


#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Chonps
{
	static RendererAPI* s_RendererAPI;

	void renderDraw(const size_t& count)
	{
		s_RendererAPI->Draw(count);
	}

	void renderBeginScene()
	{

	}

	void renderEndScene()
	{

	}

	void renderClear()
	{
		s_RendererAPI->Clear();
	}

	void renderClearColor(const float r, const float g, const float b, const float w /*= 0.0f*/)
	{
		s_RendererAPI->SetClearColor(r, g, b, w);
	}

	void setRendererAPI()
	{
		s_RendererAPI = createRendererAPI();
	}

	RendererAPI* getRendererAPI()
	{
		return s_RendererAPI;
	}
}