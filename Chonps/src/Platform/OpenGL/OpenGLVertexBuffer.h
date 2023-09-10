#ifndef HG_CHONPS_OPENGL_VERTEX_BUFFER_H
#define HG_CHONPS_OPENGL_VERTEX_BUFFER_H

#include "Graphics/VertexBuffer.h"

namespace Chonps
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		// VBO constructor for static arrays
		OpenGLVertexBuffer(float* vertices, uint32_t size, BufferState bufferState = BufferState::Static);
		// VBO constructor for dynamic array
		OpenGLVertexBuffer(std::vector<vertex>& vertices, BufferState bufferState = BufferState::Static);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetCount() const override { return m_Count; }
		virtual float* GetVertices() const override { return m_Vertices; }
		virtual uint32_t GetSize() const override { return m_Size; }

		virtual void Subdata(float* vertices, uint32_t size) override;
		virtual void Subdata(std::vector<vertex>& vertices) override;

		virtual uint32_t id() const override { return m_ID; }

	private:
		uint32_t m_ID;
		uint32_t m_Count;
		float* m_Vertices;
		uint32_t m_Size;
	};
}

#endif