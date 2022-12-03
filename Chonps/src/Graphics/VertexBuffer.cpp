#include "cepch.h"
#include "VertexBuffer.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"

namespace Chonps
{
	uint32_t getShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float: return sizeof(float);
			case ShaderDataType::Float1: return sizeof(float);
			case ShaderDataType::Float2: return sizeof(float) * 2;
			case ShaderDataType::Float3: return sizeof(float) * 3;
			case ShaderDataType::Float4: return sizeof(float) * 4;
			case ShaderDataType::Mat3: return sizeof(float) * 3 * 3;
			case ShaderDataType::Mat4: return sizeof(float) * 4 * 4;
			case ShaderDataType::Int: return sizeof(int);
			case ShaderDataType::Int1: return sizeof(int);
			case ShaderDataType::Int2: return sizeof(int) * 2;
			case ShaderDataType::Int3: return sizeof(int) * 3;
			case ShaderDataType::Int4: return sizeof(int) * 4;
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
			case ShaderDataType::Float1: return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
			case ShaderDataType::Mat2: return 2 * 2;
			case ShaderDataType::Mat3: return 3 * 3;
			case ShaderDataType::Mat4: return 4 * 4;
			case ShaderDataType::Int: return 1;
			case ShaderDataType::Int1: return 1;
			case ShaderDataType::Int2: return 2;
			case ShaderDataType::Int3: return 3;
			case ShaderDataType::Int4: return 4;
			case ShaderDataType::Bool: return 1;
		}

		CHONPS_CORE_ASSERT(false, "Count - Unknown shader data type!");
		return 0;
	}

	VertexBuffer* createVertexBuffer(float* vertices, uint32_t size)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: VBO: createVBO(vertices, size) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLVertexBuffer(vertices, size);
				break;
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
		CHONPS_CORE_ERROR("ERROR: VBO: Could not create VBO!");
		return nullptr;
	}

	VertexBuffer* createVertexBuffer(std::vector<vertextb>& vertices)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: VBO: createVertexVBO(vertices) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLVertexBuffer(vertices);
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
		CHONPS_CORE_ERROR("ERROR: VBO: Could not create VBO!");
		return nullptr;
	}
}