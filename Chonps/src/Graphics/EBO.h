#ifndef EBO_H
#define EBO_H

namespace Chonps
{
	class EBO
	{
	public:
		// EBO constructor for static arrays
		EBO(uint32_t* indices, size_t size) {}
		// EBO constructor for dynamic arrays
		EBO(std::vector<uint32_t>& indices) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

	};

	EBO* createEBO(uint32_t* indices, size_t size);
	EBO* createVertexEBO(std::vector<uint32_t>& indices);
}

#endif