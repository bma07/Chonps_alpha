#include "cepch.h"
#include "EBO.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLEBO.h"

namespace Chonps
{
	EBO* createEBO(uint32_t* indices, size_t size)
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createEBO(indices, size) - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLEBO(indices, size);
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
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}

	EBO* createVertexEBO(std::vector<uint32_t>& indices)
	{
		switch (getGraphicsContext())
		{
		case API::None:
		{
			CHONPS_CORE_WARN("WANRING: EBO: createEBO(indices) - No graphics API selected beforehand!");
			break;
		}

		case API::OpenGL:
		{
			return new OpenGLEBO(indices);
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
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}
}