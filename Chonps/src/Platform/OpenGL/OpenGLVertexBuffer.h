#ifndef OPENGL_VERTEX_BUFFER_H
#define OPENGL_VERTEX_BUFFER_H

#include "Graphics/VertexBuffer.h"

namespace Chonps
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		// VBO constructor for static arrays
		OpenGLVertexBuffer(float* vertices, size_t size);
		// VBO constructor for dynamic array
		OpenGLVertexBuffer(std::vector<vertex>& vertices);


		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

	private:
		uint32_t m_ID;
	};
}

#endif