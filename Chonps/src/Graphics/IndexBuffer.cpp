#include "cepch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"

namespace Chonps
{
	IndexBuffer* createIndexBuffer(uint32_t* indices, uint32_t size)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createEBO(indices, size) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLIndexBuffer(indices, size);
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
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}

	IndexBuffer* createIndexBuffer(std::vector<uint32_t>& indices)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: EBO: createVertexEBO(indices) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLIndexBuffer(indices);
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
		CHONPS_CORE_ERROR("ERROR: EBO: Could not create EBO!");
		return nullptr;
	}
}