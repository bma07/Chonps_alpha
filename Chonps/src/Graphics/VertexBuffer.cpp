#include "cepch.h"
#include "VertexBuffer.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/Vulkan/VulkanVertexBuffer.h"

namespace Chonps
{
	uint32_t getShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float: return sizeof(float);
			case ShaderDataType::Mat2: return sizeof(float) * 2 * 2;
			case ShaderDataType::Mat3: return sizeof(float) * 3 * 3;
			case ShaderDataType::Mat4: return sizeof(float) * 4 * 4;
			case ShaderDataType::Int: return sizeof(int);
			case ShaderDataType::Bool: return sizeof(bool);
		}

		CHONPS_CORE_ASSERT(false, "Size - Unknown shader data type!");
		return 0;
	}

	uint32_t getShaderDataTypeComponent(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float: return 1;
			case ShaderDataType::Mat2: return 2 * 2;
			case ShaderDataType::Mat3: return 3 * 3;
			case ShaderDataType::Mat4: return 4 * 4;
			case ShaderDataType::Int: return 1;
			case ShaderDataType::Bool: return 1;
		}

		CHONPS_CORE_ASSERT(false, "Count - Unknown shader data type!");
		return 0;
	}

	std::shared_ptr<VertexBuffer> createVertexBufferSp(float* vertices, uint32_t size)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: VBO: createVertexBufferSp(vertices, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLVertexBuffer>(vertices, size);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanVertexBuffer>(vertices, size);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: VBO: Could not create VBO!");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> createVertexBufferSp(std::vector<vertex>& vertices)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: VBO: createVertexBufferSp(vertices) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLVertexBuffer>(vertices);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanVertexBuffer>(vertices);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: VBO: Could not create VBO!");
		return nullptr;
	}

	VertexBuffer* createVertexBuffer(float* vertices, uint32_t size)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: VBO: createVertexBuffer(vertices, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLVertexBuffer(vertices, size);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanVertexBuffer(vertices, size);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: VBO: Could not create VBO!");
		return nullptr;
	}

	VertexBuffer* createVertexBuffer(std::vector<vertex>& vertices)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: VBO: createVertexBuffer(vertices) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLVertexBuffer(vertices);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanVertexBuffer(vertices);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: VBO: Could not create VBO!");
		return nullptr;
	}
}