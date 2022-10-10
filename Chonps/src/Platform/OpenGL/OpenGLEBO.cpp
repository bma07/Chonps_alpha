#include "cepch.h"
#include "OpenGLEBO.h"

#include <glad/glad.h>

namespace Chonps
{
	OpenGLEBO::OpenGLEBO(uint32_t* indices, size_t size)
		: EBO(indices, size), m_Count(size / sizeof(uint32_t))
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	OpenGLEBO::OpenGLEBO(std::vector<uint32_t>& indices)
		: EBO(indices)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	}

	void OpenGLEBO::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	}

	void OpenGLEBO::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLEBO::Delete()
	{
		glDeleteBuffers(1, &m_ID);
	}
}