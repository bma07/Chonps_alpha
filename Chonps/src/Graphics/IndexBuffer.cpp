#include "cepch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/Vulkan/VulkanIndexBuffer.h"

namespace Chonps
{
	std::shared_ptr<IndexBuffer> createIndexBuffer(uint32_t* indices, uint32_t size)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createIndexBuffer(indices, size) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return std::make_shared<OpenGLIndexBuffer>(indices, size);
			}

			case RenderAPI::Vulkan:
			{
				return std::make_shared<VulkanIndexBuffer>(indices, size);;
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<uint32_t>& indices)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createIndexBuffer(indices) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return std::make_shared<OpenGLIndexBuffer>(indices);
			}

			case RenderAPI::Vulkan:
			{
				return std::make_shared<VulkanIndexBuffer>(indices);
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}

	IndexBuffer* createIndexBufferRp(uint32_t* indices, uint32_t size)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createIndexBufferRp(indices, size) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLIndexBuffer(indices, size);
			}

			case RenderAPI::Vulkan:
			{
				return new VulkanIndexBuffer(indices, size);
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}

	IndexBuffer* createIndexBufferRp(std::vector<uint32_t>& indices)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createIndexBufferRp(indices) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLIndexBuffer(indices);
			}

			case RenderAPI::Vulkan:
			{
				return new VulkanIndexBuffer(indices);
			}

			case RenderAPI::DirectX:
			{
				break;
			}

		}
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}
}