#ifndef CHONPS_INDEX_BUFFER_H
#define CHONPS_INDEX_BUFFER_H

namespace Chonps
{
	class IndexBuffer
	{
	public:
		// IndexBuffer constructor for static arrays
		IndexBuffer(uint32_t* indices, uint32_t size) {}
		// IndexBuffer constructor for dynamic arrays
		IndexBuffer(std::vector<uint32_t>& indices) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetCount() const = 0;

	};
	typedef IndexBuffer IBO;

	std::shared_ptr<IndexBuffer> createIndexBuffer(uint32_t* indices, uint32_t size);
	std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<uint32_t>& indices);

	IndexBuffer* createIndexBufferRp(uint32_t* indices, uint32_t size);
	IndexBuffer* createIndexBufferRp(std::vector<uint32_t>& indices);
}

#endif