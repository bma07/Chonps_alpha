#include "cepch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Chonps
{
	std::shared_ptr<FrameBuffer> createFrameBufferSp(FrameBufferSpecification frameBufferSpecification)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: FBO: createFrameBufferSp() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLFrameBuffer>(frameBufferSpecification);
			}

			case GraphicsAPI::Vulkan:
			{
				break;
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: FBO: Could not create FBO!");
		return nullptr;
	}

	FrameBuffer* createFrameBuffer(FrameBufferSpecification frameBufferSpecification)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: FBO: createFrameBuffer() - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLFrameBuffer(frameBufferSpecification);
			}

			case GraphicsAPI::Vulkan:
			{
				break;
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: FBO: Could not create FBO!");
		return nullptr;
	}
}