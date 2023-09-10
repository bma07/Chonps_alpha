#include "cepch.h"
#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace Chonps
{
	GLenum getBufferState(BufferState drawState)
	{
		switch (drawState)
		{
			case Chonps::BufferState::Static: { return GL_STATIC_DRAW; }
			case Chonps::BufferState::Dynamic: { return GL_DYNAMIC_DRAW; }
			default:
				return GL_STATIC_DRAW;
		}
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size, BufferState bufferState)
		: VertexBuffer(vertices, size, bufferState), m_Count(size / sizeof(uint32_t)), m_Vertices(vertices), m_Size(size)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, getBufferState(bufferState));
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<vertex>& vertices, BufferState bufferState)
		: VertexBuffer(vertices, bufferState), m_Count(static_cast<uint32_t>(vertices.size())), m_Vertices((float*)vertices.data()), m_Size(static_cast<uint32_t>(sizeof(vertex)* vertices.size()))
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), getBufferState(bufferState));
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::Delete()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void OpenGLVertexBuffer::Subdata(float* vertices, uint32_t size)
	{
		m_Count = size / sizeof(uint32_t);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void OpenGLVertexBuffer::Subdata(std::vector<vertex>& vertices)
	{
		m_Count = static_cast<uint32_t>(vertices.size());
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertex), vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}