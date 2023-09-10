#include "cepch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

#include "Graphics/RendererBackends.h"

namespace Chonps
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t binding, uint32_t size, BufferType bufferType)
		: UniformBuffer(binding, size, bufferType), m_Type(bufferType)
	{
		GLenum bufferTypeEnum = GL_UNIFORM_BUFFER;

		switch (bufferType)
		{
			case Chonps::BufferType::None:
			{
				CHONPS_CORE_ERROR("ERROR: UNIFORM_BUFFER: No bufferType was chosen!");
				break;
			}
			case Chonps::BufferType::Uniform:
			{
				bufferTypeEnum = GL_UNIFORM_BUFFER;
				break;
			}
			case Chonps::BufferType::Storage:
			{
				bufferTypeEnum = GL_SHADER_STORAGE_BUFFER;
				break;
			}
		}

		size_t bufferSize = bufferType == BufferType::Storage ? size * getRendererBackends()->maxRenderEntities : size;

		glCreateBuffers(1, &m_ID);
		glNamedBufferData(m_ID, static_cast<GLsizeiptr>(bufferSize), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(bufferTypeEnum, binding, m_ID);
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