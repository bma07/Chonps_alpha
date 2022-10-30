#include "cepch.h"
#include "VertexArray.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Chonps
{
	VertexArray* createVertexArray()
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
				return new OpenGLVertexArray();
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
		CHONPS_CORE_ERROR("ERROR: VAO: Could not create VAO!");
		return nullptr;
	}
}