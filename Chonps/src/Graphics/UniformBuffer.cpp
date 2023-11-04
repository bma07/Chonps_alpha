#include "cepch.h"
#include "UniformBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Chonps
{
	BufferLayout createBufferLayout(BufferBindingCreateLayoutsInfo* bufferBindings, uint32_t setIndex)
	{
		if (getGraphicsContext() == GraphicsContext::Vulkan)
			return vks::vkImplCreateBufferLayout(bufferBindings, setIndex);

		return { nullptr, 0 };
	}

	std::shared_ptr<UniformBuffer> createUniformBufferSp(uint32_t binding, uint32_t size, BufferType bufferType)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(UniformBuffer, "createUniformBufferSp(binding, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return std::make_shared<OpenGLUniformBuffer>(binding, size, bufferType); }

			case GraphicsContext::Vulkan: { return std::make_shared<VulkanUniformBuffer>(binding, size, bufferType); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(UniformBuffer, "Could not create Uniform Buffer!");
		return nullptr;
	}

	UniformBuffer* createUniformBuffer(uint32_t binding, uint32_t size, BufferType bufferType)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(UniformBuffer, "createUniformBuffer(binding, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return new OpenGLUniformBuffer(binding, size, bufferType); }

			case GraphicsContext::Vulkan: { return new VulkanUniformBuffer(binding, size, bufferType); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(UniformBuffer, "Could not create Uniform Buffer!");
		return nullptr;
	}
}