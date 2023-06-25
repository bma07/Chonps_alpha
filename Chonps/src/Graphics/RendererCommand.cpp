#include "cepch.h"
#include "RendererCommand.h"

#include "RendererBackends.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Chonps
{
	// Sets and init specific render function calls before drawing vertices
	void renderInit()
	{
		static bool s_Init = false;

		if (!s_Init)
		{
			getRendererAPI()->Init();
			s_Init = true;
		}
		else CHONPS_CORE_WARN("WARNING: Attempting to call renderInit more than once! renderInit must be called only one time to initialize rendering.");
	}

	// Clear function clears the window viewport with all of the vertices drawn on it
	void renderClear()
	{
		getRendererAPI()->Clear();
	}

	// Clears the window and set the color of the viewport/background
	void renderClearColor(const float r, const float g, const float b, const float w /*= 0.0f*/)
	{
		getRendererAPI()->ClearColor(r, g, b, w);
	}

	// Prepares any data related to rendering before the rendering begins, 
	// this function is usually called before the main render loop and after
	// all vertex, index, texture data, ect. have been created.
	void renderPrepareDraw()
	{
		GraphicsAPI GraphicsAPI = getGraphicsAPI();

		if (GraphicsAPI == GraphicsAPI::Vulkan)
			vkSpec::vkImplPrepareDraw();
	}

	void renderBeginNextFrame()
	{
		getRendererAPI()->BeginNextFrame();
	}

	void renderDrawSubmit()
	{
		getRendererAPI()->DrawSubmit();
	}

	void renderFrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height)
	{
		getRendererAPI()->FrameBufferBlit(readFBO, drawFBO, width, height);
	}
}