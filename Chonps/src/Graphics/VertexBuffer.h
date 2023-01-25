#ifndef CHONPS_VERTEX_BUFFER_H
#define CHONPS_VERTEX_BUFFER_H

#include "Core/DataTypes.h"

#include <vector>

namespace Chonps
{
	enum class ShaderDataType
	{
		None = 0,
		Float,
		Mat2, Mat3, Mat4,
		Int,
		Bool
	};

	typedef ShaderDataType SDT;

	uint32_t getShaderDataTypeSize(ShaderDataType type);
	uint32_t getShaderDataTypeComponent(ShaderDataType type);

	class VertexBuffer
	{
	public:
		// VBO constructor for static arrays
		VertexBuffer(float* vertices, uint32_t size) {}
		// VBO constructor for dynamic array
		VertexBuffer(std::vector<vertex>& vertices) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetCount() const = 0;

	};
	typedef VertexBuffer VBO;

	std::shared_ptr<VertexBuffer> createVertexBuffer(float* vertices, uint32_t size);
	std::shared_ptr<VertexBuffer> createVertexBuffer(std::vector<vertex>& vertices);


	VertexBuffer* createVertexBufferRp(float* vertices, uint32_t size);
	VertexBuffer* createVertexBufferRp(std::vector<vertex>& vertices);
}

#endif