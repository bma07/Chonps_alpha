#include "cepch.h"
#include "UniformBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Chonps
{
	std::shared_ptr<UniformBuffer> createUniformBufferSp(uint32_t binding, uint32_t size)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: UBO: createUniformBufferSp(binding, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLUniformBuffer>(binding, size);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanUniformBuffer>(binding, size);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: UBO: Could not create Uniform Buffer!");
		return nullptr;
	}

	UniformBuffer* createUniformBuffer(uint32_t binding, uint32_t size)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: UBO: createUniformBuffer(binding, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLUniformBuffer(binding, size);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanUniformBuffer(binding, size);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: UBO: Could not create Uniform Buffer!");
		return nullptr;
	}
}