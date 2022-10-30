#ifndef CHONPS_INDEX_BUFFER_H
#define CHONPS_INDEX_BUFFER_H

namespace Chonps
{
	class IndexBuffer
	{
	public:
		// IndexBuffer constructor for static arrays
		IndexBuffer(uint32_t* indices, size_t size) {}
		// IndexBuffer constructor for dynamic arrays
		IndexBuffer(std::vector<uint32_t>& indices) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetCount() const = 0;

	};
	typedef IndexBuffer IBO;

	IndexBuffer* createIndexBuffer(uint32_t* indices, size_t size);
	IndexBuffer* createIndexBuffer(std::vector<uint32_t>& indices);
}

#endif