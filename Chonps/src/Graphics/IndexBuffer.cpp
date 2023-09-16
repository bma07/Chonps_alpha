#include "cepch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/Vulkan/VulkanIndexBuffer.h"

namespace Chonps
{
	std::shared_ptr<IndexBuffer> createIndexBufferSp(uint32_t* indices, uint32_t size, BufferState bufferState)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(IndexBuffer, "createIndexBufferSp(indices, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return std::make_shared<OpenGLIndexBuffer>(indices, size, bufferState); }

			case GraphicsAPI::Vulkan: { return std::make_shared<VulkanIndexBuffer>(indices, size, bufferState); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(IndexBuffer, "Could not create EBO!");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> createIndexBufferSp(std::vector<uint32_t>& indices, BufferState bufferState)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(IndexBuffer, "createIndexBufferSp(indices) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return std::make_shared<OpenGLIndexBuffer>(indices, bufferState); }

			case GraphicsAPI::Vulkan: { return std::make_shared<VulkanIndexBuffer>(indices, bufferState); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(IndexBuffer, "Could not create EBO!");
		return nullptr;
	}

	IndexBuffer* createIndexBuffer(uint32_t* indices, uint32_t size, BufferState bufferState)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(IndexBuffer, "createIndexBuffer(indices, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return new OpenGLIndexBuffer(indices, size, bufferState); }

			case GraphicsAPI::Vulkan: { return new VulkanIndexBuffer(indices, size, bufferState); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(IndexBuffer, "Could not create EBO!");
		return nullptr;
	}

	IndexBuffer* createIndexBuffer(std::vector<uint32_t>& indices, BufferState bufferState)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(IndexBuffer, "createIndexBuffer(indices) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return new OpenGLIndexBuffer(indices, bufferState); }

			case GraphicsAPI::Vulkan: { return new VulkanIndexBuffer(indices, bufferState); }

			case GraphicsAPI::DirectX:
			{
				break;
			}

		}
		CHONPS_CORE_LOG_ERROR(IndexBuffer, "Could not create EBO!");
		return nullptr;
	}
}