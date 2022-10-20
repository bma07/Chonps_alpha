#include "cepch.h"
#include "FBO.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFBO.h"

namespace Chonps
{
	FBO* createFBO(unsigned int width, unsigned int height)
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: FBO: createFBO() - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLFBO(width, height);
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
		CHONPS_CORE_ERROR("ERROR: FBO: Could not create FBO!");
		return nullptr;
	}
}