#include "cepch.h"
#include "VAO.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLVAO.h"

namespace Chonps
{
	VAO* createVAO()
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: VAO: createVAO() - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLVAO();
				break;
			}

			case API::Vulkan:
			{
				break;
			}

			case API::DirectX:
			{
				break;
			}

			default:
			{
				CHONPS_CORE_ERROR("Cannot find the graphics API selected!");
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: VAO: Could not create VAO!");
		return nullptr;
	}
}