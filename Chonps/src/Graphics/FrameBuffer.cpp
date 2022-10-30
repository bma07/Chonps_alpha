#include "cepch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Chonps
{
	FrameBuffer* createFrameBuffer(FrameBufferSpecification frameBufferSpecification)
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: FBO: createFBO() - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLFrameBuffer(frameBufferSpecification);
			}

			case API::Vulkan:
			{
				break;
			}

			case API::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: FBO: Could not create FBO!");
		return nullptr;
	}
}