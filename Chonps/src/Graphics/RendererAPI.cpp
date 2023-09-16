#include "cepch.h"
#include "RendererAPI.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Chonps
{
	static GraphicsAPI s_GraphicsAPI = GraphicsAPI::None;

	// Create the renderering API that holds the renderering context and implementation
	static RendererAPI* s_RendererAPI = nullptr;

	bool setRenderContext(GraphicsAPI api)
	{
		s_GraphicsAPI = api;

		if (api == GraphicsAPI::None)
		{
			CHONPS_CORE_LOG_WARN(GraphicsAPI, "No graphics API selected!");
			return false;
		}
		CHONPS_CORE_INFO("Rendering Context Initialized: {0}", getGraphicsAPIName());
		return true;
	}

	GraphicsAPI getGraphicsAPI()
	{
		return s_GraphicsAPI;
	}

	std::string getGraphicsAPIName()
	{
		switch (getGraphicsAPI())
		{
			case Chonps::GraphicsAPI::None: return "None";
			case Chonps::GraphicsAPI::OpenGL: return "OpenGL";
			case Chonps::GraphicsAPI::Vulkan: return "Vulkan";
			case Chonps::GraphicsAPI::DirectX: return "DirectX";
		}

		CHONPS_CORE_LOG_ERROR(GraphicsAPI, "Cannot find the graphics API selected!");
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

		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(GraphicsAPI, "createRendererAPI() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				renderAPI = new OpenGLRendererAPI();
				break;
			}

			case GraphicsAPI::Vulkan:
			{
				renderAPI = new VulkanRendererAPI();
				break;
			}

			case GraphicsAPI::DirectX:
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

	void destroyRendererAPI()
	{
		if (s_RendererAPI != nullptr)
			delete s_RendererAPI;
		else CHONPS_CORE_LOG_WARN(RendererAPI, "RendererAPI is nullptr! Cannot destroy RendererAPI!");
	}

	void textureBinding(uint32_t textureBinding, uint32_t samplerBinding, uint32_t frameBufferBinding, uint32_t cubemapBinding)
	{
		GraphicsAPI GraphicsAPI = getGraphicsAPI();

		if (GraphicsAPI == GraphicsAPI::Vulkan)
			vks::vkImplTextureBinding(textureBinding, samplerBinding, frameBufferBinding, cubemapBinding);
	}
}
