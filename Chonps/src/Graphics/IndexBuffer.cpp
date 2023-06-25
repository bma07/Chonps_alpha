#include "cepch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/Vulkan/VulkanIndexBuffer.h"

namespace Chonps
{
	std::shared_ptr<IndexBuffer> createIndexBufferSp(uint32_t* indices, uint32_t size)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createIndexBufferSp(indices, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLIndexBuffer>(indices, size);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanIndexBuffer>(indices, size);;
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> createIndexBufferSp(std::vector<uint32_t>& indices)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createIndexBufferSp(indices) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLIndexBuffer>(indices);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanIndexBuffer>(indices);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}

	IndexBuffer* createIndexBuffer(uint32_t* indices, uint32_t size)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createIndexBuffer(indices, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLIndexBuffer(indices, size);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanIndexBuffer(indices, size);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}

	IndexBuffer* createIndexBuffer(std::vector<uint32_t>& indices)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createIndexBuffer(indices) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLIndexBuffer(indices);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanIndexBuffer(indices);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}

		}
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}
}