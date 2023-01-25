#include "cepch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Chonps
{
	GLenum getShaderDataTypeConvertOpenGL(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::None: return GL_FLOAT;
			case ShaderDataType::Float: return GL_FLOAT;
			case ShaderDataType::Mat2: return GL_FLOAT;
			case ShaderDataType::Mat3: return GL_FLOAT;
			case ShaderDataType::Mat4: return GL_FLOAT;
			case ShaderDataType::Int: return GL_INT;
			case ShaderDataType::Bool: return GL_BOOL;
		}

		CHONPS_CORE_ASSERT(false, "Convert OpenGL - Unkown shader data type!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &m_ID);
	}

	void OpenGLVertexArray::LinkVertexBuffer(VertexBuffer* VBO, uint32_t layout, uint32_t numComponents, ShaderDataType type, uint32_t stride, void* offset)
	{
		VBO->Bind();
		glEnableVertexAttribArray(layout);
		glVertexAttribPointer(layout, numComponents, getShaderDataTypeConvertOpenGL(type), GL_FALSE, (GLsizei)stride, offset);
		VBO->Unbind();
		m_VertexCount = VBO->GetCount();
	}

	void OpenGLVertexArray::LinkIndexBuffer(IndexBuffer* IBO)
	{
		m_IndexCount = IBO->GetCount();
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_ID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::Delete()
	{
		glDeleteVertexArrays(1, &m_ID);
	}
}