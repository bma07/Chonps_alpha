#ifndef CHONPS_VERTEX_ARRAY_H
#define CHONPS_VERTEX_ARRAY_H

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Chonps
{
	class VertexArray
	{
	public:
		VertexArray() {}

		virtual void LinkVertexBuffer(VertexBuffer* VBO, uint32_t layout, uint32_t numComponents, ShaderDataType type, size_t stride, void* offset) = 0;
		virtual void LinkVertexBuffer(VertexBuffer* VBO, uint32_t layout, ShaderDataType numComponents, ShaderDataType type, size_t stride, void* offset) = 0;
		virtual void LinkIndexBuffer(IndexBuffer* EBO) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetIndexCount() const = 0;

	};
	typedef VertexArray VAO;

	VertexArray* createVertexArray();
}

#endif