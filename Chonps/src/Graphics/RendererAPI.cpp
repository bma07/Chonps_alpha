#include "cepch.h"
#include "RendererAPI.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Chonps
{
	static RenderAPI s_API = RenderAPI::None;
	
	// Create the renderering API that holds the renderering context and implementation
	static RendererAPI* s_RendererAPI;

	bool RendererAPI::m_GammaCorrection = false;
	float RendererAPI::m_Gamma = 2.2f;

	bool setRenderContext(RenderAPI api /*= API::OpenGL*/)
	{
		s_API = api;
		setRendererAPI();

		if (api == RenderAPI::None)
		{
			CHONPS_CORE_WARN("No graphics API selected!");
			return false;
		}
		CHONPS_CORE_INFO("Rendering Context Initialized: {0}", getGraphicsContextName());
		return true;
	}

	RenderAPI getGraphicsContext()
	{
		return s_API;
	}

	std::string getGraphicsContextName()
	{
		switch (getGraphicsContext())
		{
			case Chonps::RenderAPI::None: return "None";
			case Chonps::RenderAPI::OpenGL: return "OpenGL";
			case Chonps::RenderAPI::Vulkan: return "Vulkan";
			case Chonps::RenderAPI::DirectX: return "DirectX";
		}

		CHONPS_CORE_ERROR("Cannot find the graphics API selected!");
		return "null";
	}

	void setRendererAPI()
	{
		s_RendererAPI = createRendererAPI();
	}

	// Sets and updates the RenderingAPI to the graphics API currently selected
	RendererAPI* getRendererAPI()
	{
		return s_RendererAPI;
	}

	RendererAPI* createRendererAPI()
	{
		switch (getGraphicsContext())
		{
		case RenderAPI::None:
		{
			CHONPS_CORE_WARN("WANRING: RENDERER_API: createRendererAPI() - No graphics API selected beforehand!");
			break;
		}

		case RenderAPI::OpenGL:
		{
			return new OpenGLRendererAPI();
		}

		case RenderAPI::Vulkan:
		{
			break;
		}

		case RenderAPI::DirectX:
		{
			break;
		}
		}
		CHONPS_CORE_ERROR("ERROR: RENDERER_API: Could not get rendering API!");
		return nullptr;
	}

}