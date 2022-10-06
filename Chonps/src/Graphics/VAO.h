#ifndef VAO_H
#define VAO_H

#include "VBO.h"

namespace Chonps
{
	class VAO
	{
	public:
		VAO() {}

		virtual void LinkAttributes(VBO& VBO, uint32_t layout, uint32_t numComponents, ShaderDataType type, size_t stride, void* offset) = 0;
		virtual void LinkAttributes(VBO& VBO, uint32_t layout, ShaderDataType numComponents, ShaderDataType type, size_t stride, void* offset) = 0;
		
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

	};

	VAO* createVAO();
}

#endif