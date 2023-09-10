#include "cepch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Platform/Vulkan/VulkanFrameBuffer.h"

namespace Chonps
{
	std::shared_ptr<FrameBuffer> createFrameBufferSp(FrameBufferSpecificationInfo frameBufferSpecificationInfo)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: FBO: createFrameBufferSp() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return std::make_shared<OpenGLFrameBuffer>(frameBufferSpecificationInfo); }

			case GraphicsAPI::Vulkan: { return std::make_shared<VulkanFrameBuffer>(frameBufferSpecificationInfo); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: FBO: Could not create FBO!");
		return nullptr;
	}

	FrameBuffer* createFrameBuffer(FrameBufferSpecificationInfo frameBufferSpecificationInfo)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: FBO: createFrameBuffer() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL: { return new OpenGLFrameBuffer(frameBufferSpecificationInfo); }

			case GraphicsAPI::Vulkan: { return new VulkanFrameBuffer(frameBufferSpecificationInfo); }

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: FBO: Could not create FBO!");
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