#ifndef HG_CHONPS_INDEX_BUFFER_H
#define HG_CHONPS_INDEX_BUFFER_H

#include "VertexBuffer.h"

namespace Chonps
{
	class IndexBuffer
	{
	public:
		// IndexBuffer constructor for static arrays
		IndexBuffer(uint32_t* indices, uint32_t size, BufferState bufferState = BufferState::Static) {}
		// IndexBuffer constructor for dynamic arrays
		IndexBuffer(std::vector<uint32_t>& indices, BufferState bufferState = BufferState::Static) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetCount() const = 0;
		virtual uint32_t* GetIndices() const = 0;
		virtual uint32_t GetSize() const = 0;

		virtual void Subdata(uint32_t* indices, uint32_t size) = 0;
		virtual void Subdata(std::vector<uint32_t>& indices) = 0;

		virtual uint32_t id() const = 0;
	};
	typedef IndexBuffer IBO;

	std::shared_ptr<IndexBuffer> createIndexBufferSp(uint32_t* indices, uint32_t size, BufferState bufferState = BufferState::Static);
	std::shared_ptr<IndexBuffer> createIndexBufferSp(std::vector<uint32_t>& indices, BufferState bufferState = BufferState::Static);

	IndexBuffer* createIndexBuffer(uint32_t* indices, uint32_t size, BufferState bufferState = BufferState::Static);
	IndexBuffer* createIndexBuffer(std::vector<uint32_t>& indices, BufferState bufferState = BufferState::Static);
}

#endif