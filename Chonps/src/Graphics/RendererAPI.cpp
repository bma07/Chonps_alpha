#include "cepch.h"
#include "RendererAPI.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Chonps
{
	static GraphicsContext s_GraphicsContext = GraphicsContext::None;

	// Create the renderering API that holds the renderering context and implementation
	static RendererAPI* s_RendererAPI = nullptr;

	bool setGraphicsContext(GraphicsContext api)
	{
		s_GraphicsContext = api;

		if (api == GraphicsContext::None)
		{
			CHONPS_CORE_LOG_WARN(GraphicsContext, "No graphics API selected!");
			return false;
		}
		CHONPS_CORE_INFO("Rendering Context Initialized: {0}", getGraphicsContextName());
		return true;
	}

	GraphicsContext getGraphicsContext()
	{
		return s_GraphicsContext;
	}

	std::string getGraphicsContextName()
	{
		switch (getGraphicsContext())
		{
			case Chonps::GraphicsContext::None: return "None";
			case Chonps::GraphicsContext::OpenGL: return "OpenGL";
			case Chonps::GraphicsContext::Vulkan: return "Vulkan";
			case Chonps::GraphicsContext::DirectX: return "DirectX";
		}

		CHONPS_CORE_LOG_ERROR(GraphicsContext, "Cannot find the graphics API selected!");
		return "null";
	}

	// Sets and updates the RenderingAPI to the graphics API currently selected
	RendererAPI* getRendererAPI()
	{
		return s_RendererAPI;
	}

	bool createRendererAPI(RendererBackends* rendererBackends)
	{
		RendererAPI* renderAPI = nullptr;

		if (rendererBackends == nullptr)
		{
			CHONPS_CORE_LOG_ERROR(RendererAPI, "createRendererAPI() - Renderer Backends was nullptr!");
			return false;
		}
		else
			setRendererBackends(*rendererBackends);

		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(GraphicsContext, "createRendererAPI() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL:
			{
				renderAPI = new OpenGLRendererAPI();
				break;
			}

			case GraphicsContext::Vulkan:
			{
				renderAPI = new VulkanRendererAPI();
				break;
			}

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		s_RendererAPI = renderAPI;

		if (renderAPI == nullptr)
		{
			CHONPS_CORE_LOG_ERROR(RendererAPI, "Could not get rendering API!");
			return false;
		}

		return true;
	}

	bool destroyRendererAPI()
	{
		if (s_RendererAPI != nullptr)
		{
			delete s_RendererAPI;
			return true;
		}
		else
		{
			CHONPS_CORE_LOG_WARN(RendererAPI, "RendererAPI is nullptr! Cannot destroy RendererAPI!");
			return false;
		}
	}

	void textureBinding(uint32_t textureBinding, uint32_t samplerBinding, uint32_t frameBufferBinding, uint32_t cubemapBinding)
	{
		GraphicsContext GraphicsContext = getGraphicsContext();

		if (GraphicsContext == GraphicsContext::Vulkan)
			vks::vkImplTextureBinding(textureBinding, samplerBinding, frameBufferBinding, cubemapBinding);
	}
}
