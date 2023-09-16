#include "cepch.h"
#include "VertexArray.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Chonps
{
	std::shared_ptr<VertexArray> createVertexArraySp()
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(VertexArray, "createVertexArraySp() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return std::make_shared<OpenGLVertexArray>(); }

			case GraphicsAPI::Vulkan: { return std::make_shared<VulkanVertexArray>(); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(VertexArray, "Could not create VAO!");
		return nullptr;
	}

	VertexArray* createVertexArray()
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(VertexArray, "createVertexArray() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return new OpenGLVertexArray(); }

			case GraphicsAPI::Vulkan: { return new VulkanVertexArray(); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(VertexArray, "Could not create VAO!");
		return nullptr;
	}
}