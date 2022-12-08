#include "cepch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Chonps
{
	std::shared_ptr<FrameBuffer> createFrameBuffer(FrameBufferSpecification frameBufferSpecification)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: FBO: createFrameBuffer() - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return std::make_shared<OpenGLFrameBuffer>(frameBufferSpecification);
			}

			case RenderAPI::Vulkan:
			{
				break;
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: FBO: Could not create FBO!");
		return nullptr;
	}

	FrameBuffer* createFrameBufferRp(FrameBufferSpecification frameBufferSpecification)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: FBO: createFrameBufferRp() - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLFrameBuffer(frameBufferSpecification);
			}

			case RenderAPI::Vulkan:
			{
				break;
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: FBO: Could not create FBO!");
		return nullptr;
	}
}