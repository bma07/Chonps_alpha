#include "cepch.h"
#include "OpenGLVBO.h"

#include <glad/glad.h>

namespace Chonps
{
	OpenGLVBO::OpenGLVBO(float* vertices, size_t size)
		: VBO(vertices, size)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVBO::OpenGLVBO(std::vector<vertex>& vertices)
		: VBO(vertices)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);
	}

	void OpenGLVBO::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	}

	void OpenGLVBO::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVBO::Delete()
	{
		glDeleteBuffers(1, &m_ID);
	}
}