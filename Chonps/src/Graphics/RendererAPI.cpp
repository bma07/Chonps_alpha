#include "cepch.h"
#include "RendererAPI.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Chonps
{
	static API s_API = API::None;

	bool RendererAPI::m_GammaCorrection = false;
	float RendererAPI::m_Gamma = 2.2f;

	bool setRenderAPI(API api /*= API::OpenGL*/)
	{
		s_API = api;
		setRendererAPI();

		if (api == API::None)
		{
			CHONPS_CORE_WARN("No graphics API selected!");
			return false;
		}
		CHONPS_CORE_INFO("Rendering Context Initialized: {0}", getGraphicsContextName());
		return true;
	}

	API getGraphicsContext()
	{
		return s_API;
	}

	std::string getGraphicsContextName()
	{
		switch (getGraphicsContext())
		{
			case Chonps::API::None: return "None";
			case Chonps::API::OpenGL: return "OpenGL";
			case Chonps::API::Vulkan: return "Vulkan";
			case Chonps::API::DirectX: return "DirectX";
		}

		CHONPS_CORE_ERROR("Cannot find the graphics API selected!");
		return "null";
	}

	RendererAPI* createRendererAPI()
	{
		switch (getGraphicsContext())
		{
		case API::None:
		{
			CHONPS_CORE_WARN("WANRING: RENDERER_API: createRendererAPI() - No graphics API selected beforehand!");
			break;
		}

		case API::OpenGL:
		{
			return new OpenGLRendererAPI();
		}

		case API::Vulkan:
		{
			break;
		}

		case API::DirectX:
		{
			break;
		}
		}
		CHONPS_CORE_ERROR("ERROR: RENDERER_API: Could not get rendering API!");
		return nullptr;
	}

}