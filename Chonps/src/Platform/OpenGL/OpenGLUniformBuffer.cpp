#include "cepch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Chonps
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t binding, uint32_t size)
		: UniformBuffer(binding, size)
	{
		glCreateBuffers(1, &m_ID);
		glNamedBufferData(m_ID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_ID);
	}

	void OpenGLUniformBuffer::Bind(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_ID, offset, size, data);
	}

	void OpenGLUniformBuffer::Delete()
	{
		glDeleteBuffers(1, &m_ID);
	}
}