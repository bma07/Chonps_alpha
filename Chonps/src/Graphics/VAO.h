#ifndef VAO_H
#define VAO_H

#include "VBO.h"
#include "EBO.h"

namespace Chonps
{
	class VAO
	{
	public:
		VAO() {}

		virtual void LinkVBO(VBO* VBO, uint32_t layout, uint32_t numComponents, ShaderDataType type, size_t stride, void* offset) = 0;
		virtual void LinkVBO(VBO* VBO, uint32_t layout, ShaderDataType numComponents, ShaderDataType type, size_t stride, void* offset) = 0;
		virtual void LinkEBO(EBO* EBO) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetIndexCount() const = 0;

	};

	VAO* createVAO();
}

#endif