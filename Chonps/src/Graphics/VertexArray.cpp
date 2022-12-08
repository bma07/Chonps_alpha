#include "cepch.h"
#include "VertexArray.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Chonps
{
	std::shared_ptr<VertexArray> createVertexArray()
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: VAO: createVertexArray() - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return std::make_shared<OpenGLVertexArray>();
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
		CHONPS_CORE_ERROR("ERROR: VAO: Could not create VAO!");
		return nullptr;
	}

	VertexArray* createVertexArrayRp()
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: VAO: createVertexArrayRp() - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLVertexArray();
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
		CHONPS_CORE_ERROR("ERROR: VAO: Could not create VAO!");
		return nullptr;
	}
}