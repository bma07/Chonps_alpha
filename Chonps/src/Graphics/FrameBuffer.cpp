#include "cepch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Platform/Vulkan/VulkanFrameBuffer.h"

namespace Chonps
{
	std::shared_ptr<FrameBuffer> createFrameBufferSp(FrameBufferSpecificationInfo frameBufferSpecificationInfo, uint32_t setIndex)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(FrameBuffer, "createFrameBufferSp() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return std::make_shared<OpenGLFrameBuffer>(frameBufferSpecificationInfo, setIndex); }

			case GraphicsContext::Vulkan: { return std::make_shared<VulkanFrameBuffer>(frameBufferSpecificationInfo, setIndex); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(FrameBuffer, "Could not create FBO!");
		return nullptr;
	}

	FrameBuffer* createFrameBuffer(FrameBufferSpecificationInfo frameBufferSpecificationInfo, uint32_t setIndex)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(FrameBuffer, "createFrameBuffer() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return new OpenGLFrameBuffer(frameBufferSpecificationInfo, setIndex); }

			case GraphicsContext::Vulkan: { return new VulkanFrameBuffer(frameBufferSpecificationInfo, setIndex); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(FrameBuffer, "Could not create FBO!");
		return nullptr;
	}

	RenderPass retrieveRenderPass(FrameBuffer* frameBuffer)
	{
		if (getGraphicsContext() == GraphicsContext::Vulkan)
		{
			VulkanFrameBuffer* vkFrameBuffer = static_cast<VulkanFrameBuffer*>(frameBuffer);
			return vkFrameBuffer->getRenderPass();
		}

		return nullptr;
	}
}