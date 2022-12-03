#ifndef OPENGL_VERTEX_BUFFER_H
#define OPENGL_VERTEX_BUFFER_H

#include "Graphics/VertexBuffer.h"

namespace Chonps
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		// VBO constructor for static arrays
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		// VBO constructor for dynamic array
		OpenGLVertexBuffer(std::vector<vertextb>& vertices);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_ID;
		uint32_t m_Count;
	};
}

#endif