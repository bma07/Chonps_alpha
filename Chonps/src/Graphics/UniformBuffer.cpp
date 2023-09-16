#include "cepch.h"
#include "UniformBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Chonps
{
	BufferLayout createBufferLayout(BufferBindingCreateLayoutsInfo* bufferBindings, uint32_t setIndex)
	{
		if (getGraphicsAPI() == GraphicsAPI::Vulkan)
			return vks::vkImplCreateBufferLayout(bufferBindings, setIndex);

		return { nullptr, 0 };
	}

	PushConstantRange createPushConstantRange(uint32_t size, uint32_t offset, ShaderStage shaderStage)
	{
		if (getGraphicsAPI() == GraphicsAPI::Vulkan)
			return vks::vkImplCreatePushConstantRange(size, offset, shaderStage);

		return { nullptr };
	}

	std::shared_ptr<UniformBuffer> createUniformBufferSp(uint32_t binding, uint32_t size, BufferType bufferType)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(UniformBuffer, "createUniformBufferSp(binding, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return std::make_shared<OpenGLUniformBuffer>(binding, size, bufferType); }

			case GraphicsAPI::Vulkan: { return std::make_shared<VulkanUniformBuffer>(binding, size, bufferType); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(UniformBuffer, "Could not create Uniform Buffer!");
		return nullptr;
	}

	UniformBuffer* createUniformBuffer(uint32_t binding, uint32_t size, BufferType bufferType)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(UniformBuffer, "createUniformBuffer(binding, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return new OpenGLUniformBuffer(binding, size, bufferType); }

			case GraphicsAPI::Vulkan: { return new VulkanUniformBuffer(binding, size, bufferType); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(UniformBuffer, "Could not create Uniform Buffer!");
		return nullptr;
	}
}