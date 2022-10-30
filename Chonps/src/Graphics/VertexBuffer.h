#ifndef CHONPS_VERTEX_BUFFER_H
#define CHONPS_VERTEX_BUFFER_H

#include "DataTypes.h"

#include <vector>

namespace Chonps
{
	enum class ShaderDataType
	{
		None = 0,
		Float, Float1, Float2, Float3, Float4,
		Mat2, Mat3, Mat4,
		Int, Int1, Int2, Int3, Int4,
		Bool
	};

	typedef ShaderDataType SDT;

	uint32_t getShaderDataTypeSize(ShaderDataType type);
	uint32_t getShaderDataTypeComponent(ShaderDataType type);

	class VertexBuffer
	{
	public:
		// VBO constructor for static arrays
		VertexBuffer(float* vertices, size_t size) {}
		// VBO constructor for dynamic array
		VertexBuffer(std::vector<vertex>& vertices) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

	};
	typedef VertexBuffer VBO;

	VertexBuffer* createVertexBuffer(float* vertices, uint32_t size);
	VertexBuffer* createVertexBuffer(std::vector<vertex>& vertices);
}

#endif