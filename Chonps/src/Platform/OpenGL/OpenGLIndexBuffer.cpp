#include "cepch.h"
#include "OpenGLIndexBuffer.h"

#include <glad/glad.h>

namespace Chonps
{
	GLenum getIndexDrawState(BufferState drawState)
	{
		switch (drawState)
		{
			case Chonps::BufferState::Static: { return GL_STATIC_DRAW; }
			case Chonps::BufferState::Dynamic: { return GL_DYNAMIC_DRAW; }
			default:
				return GL_STATIC_DRAW;
		}
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t size, BufferState bufferState)
		: IndexBuffer(indices, size), m_Count(size / sizeof(uint32_t)), m_Indices(indices)
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, getIndexDrawState(bufferState));
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t>& indices, BufferState bufferState)
		: IndexBuffer(indices), m_Count(static_cast<uint32_t>(indices.size())), m_Indices(indices.data())
	{
		glGenBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), getIndexDrawState(bufferState));
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

	void OpenGLIndexBuffer::Subdata(uint32_t* indices, uint32_t size)
	{
		m_Count = size / sizeof(uint32_t);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::Subdata(std::vector<uint32_t>& indices)
	{
		m_Count = static_cast<uint32_t>(indices.size());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indices.size(), indices.data());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}