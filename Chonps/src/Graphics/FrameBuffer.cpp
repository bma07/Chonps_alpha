#include "cepch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Platform/Vulkan/VulkanFrameBuffer.h"

namespace Chonps
{
	std::shared_ptr<FrameBuffer> createFrameBufferSp(FrameBufferSpecificationInfo frameBufferSpecificationInfo, uint32_t setIndex)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(FrameBuffer, "createFrameBufferSp() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return std::make_shared<OpenGLFrameBuffer>(frameBufferSpecificationInfo, setIndex); }

			case GraphicsAPI::Vulkan: { return std::make_shared<VulkanFrameBuffer>(frameBufferSpecificationInfo, setIndex); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(FrameBuffer, "Could not create FBO!");
		return nullptr;
	}

	FrameBuffer* createFrameBuffer(FrameBufferSpecificationInfo frameBufferSpecificationInfo, uint32_t setIndex)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(FrameBuffer, "createFrameBuffer() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return new OpenGLFrameBuffer(frameBufferSpecificationInfo, setIndex); }

			case GraphicsAPI::Vulkan: { return new VulkanFrameBuffer(frameBufferSpecificationInfo, setIndex); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(FrameBuffer, "Could not create FBO!");
		return nullptr;
	}

	RenderPass retrieveRenderPass(FrameBuffer* frameBuffer)
	{
		if (getGraphicsAPI() == GraphicsAPI::Vulkan)
		{
			VulkanFrameBuffer* vkFrameBuffer = static_cast<VulkanFrameBuffer*>(frameBuffer);
			return vkFrameBuffer->getRenderPass();
		}

		return nullptr;
	}
}