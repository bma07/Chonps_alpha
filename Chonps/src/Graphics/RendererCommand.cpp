#include "cepch.h"
#include "RendererCommand.h"

#include "RendererBackends.h"

namespace Chonps
{
	void RendererCommand::Init()
	{
		getRendererAPI()->Init();
	}

	// Clear function clears the window viewport with all of the vertices drawn on it
	void RendererCommand::Clear()
	{
		getRendererAPI()->Clear();
	}

	// Clears the window and set the color of the viewport/background
	void RendererCommand::ClearColor(const float r, const float g, const float b, const float w /*= 0.0f*/)
	{
		getRendererAPI()->ClearColor(r, g, b, w);
	}

	void RendererCommand::FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height)
	{
		getRendererAPI()->FrameBufferBlit(readFBO, drawFBO, width, height);
	}

	// Sets and init specific render function calls before drawing vertices
	void renderInit()
	{
		getRendererAPI()->Init();
	}

	// Clear function clears the window viewport with all of the vertices drawn on it
	void renderClear()
	{
		getRendererAPI()->Clear();
	}

	// Clears the window and set the color of the viewport/background
	void renderClearColor(const float r, const float g, const float b, const float w /*= 0.0f*/)
	{
		std::shared_ptr<RendererAPI> rendererAPI = getRendererAPI();
		float gamma = renderGetGamma();
		renderGetGammaCorrection() // Check Gamma
			? rendererAPI->ClearColor(pow(r, gamma), pow(g, gamma), pow(b, gamma), w) // if Gamma Corrected
			: rendererAPI->ClearColor(r, g, b, w); // if Gamma not corrected
	}

	void renderFrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height)
	{
		getRendererAPI()->FrameBufferBlit(readFBO, drawFBO, width, height);
	}
}