#include "cepch.h"
#include "VertexArray.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Chonps
{
	std::shared_ptr<VertexArray> createVertexArraySp()
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(VertexArray, "createVertexArraySp() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return std::make_shared<OpenGLVertexArray>(); }

			case GraphicsContext::Vulkan: { return std::make_shared<VulkanVertexArray>(); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(VertexArray, "Could not create VAO!");
		return nullptr;
	}

	VertexArray* createVertexArray()
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(VertexArray, "createVertexArray() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return new OpenGLVertexArray(); }

			case GraphicsContext::Vulkan: { return new VulkanVertexArray(); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(VertexArray, "Could not create VAO!");
		return nullptr;
	}
}