#include "cepch.h"
#include "OpenGLIndexBuffer.h"

#include <glad/glad.h>

namespace Chonps
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t size)
		: IndexBuffer(indices, size), m_Count(size / sizeof(uint32_t)), m_Indices(indices)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t>& indices)
		: IndexBuffer(indices), m_Count(static_cast<uint32_t>(indices.size())), m_Indices(indices.data())
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::Delete()
	{
		glDeleteBuffers(1, &m_ID);
	}
}