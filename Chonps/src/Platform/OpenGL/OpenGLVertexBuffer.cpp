#include "cepch.h"
#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace Chonps
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, size_t size)
		: VBO(vertices, size)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<vertex>& vertices)
		: VBO(vertices)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);
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
}