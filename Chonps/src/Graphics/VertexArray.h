#ifndef HG_CHONPS_VERTEX_ARRAY_H
#define HG_CHONPS_VERTEX_ARRAY_H

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Chonps
{
	struct VertexLayout
	{
		uint32_t layout, numComponents;
		ShaderDataType type;
		uint32_t stride;
		void* offset;
	};

	struct VertexLayoutLinkInfo
	{
		VertexLayout* pLayouts;
		uint32_t layoutCount;
	};

	struct VertexArrayCreateInfo
	{
		VertexBuffer* vertexBuffer;
		VertexLayoutLinkInfo* vertexLayouts;
		IndexBuffer* indexBuffer;
	};

	class VertexArray
	{
	public:
		VertexArray() {}

		virtual void LinkBuffers(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, VertexLayoutLinkInfo* vertexLayouts) = 0;
		virtual void LinkBuffers(VertexArrayCreateInfo* createInfo) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetIndexCount() const = 0;
		virtual uint32_t GetVertexCount() const = 0;

		virtual uint32_t id() const = 0;
	};

	std::shared_ptr<VertexArray> createVertexArraySp();
	VertexArray* createVertexArray();
}

#endif