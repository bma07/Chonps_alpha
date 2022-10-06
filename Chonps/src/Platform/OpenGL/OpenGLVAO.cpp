#include "cepch.h"
#include "OpenGLVAO.h"

#include <glad/glad.h>

namespace Chonps
{
	GLenum getShaderDataTypeConvertOpenGL(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None: return GL_FLOAT;
		case ShaderDataType::Float1: return GL_FLOAT;
		case ShaderDataType::Float2: return GL_FLOAT;
		case ShaderDataType::Float3: return GL_FLOAT;
		case ShaderDataType::Float4: return GL_FLOAT;
		case ShaderDataType::Mat3: return GL_FLOAT;
		case ShaderDataType::Mat4: return GL_FLOAT;
		case ShaderDataType::Int1: return GL_INT;
		case ShaderDataType::Int2: return GL_INT;
		case ShaderDataType::Int3: return GL_INT;
		case ShaderDataType::Int4: return GL_INT;
		case ShaderDataType::Bool: return GL_BOOL;
		}

		CHONPS_CORE_ASSERT(false, "Convert OpenGL - Unkown shader data type!");
		return 0;
	}

	OpenGLVAO::OpenGLVAO()
	{
		glGenVertexArrays(1, &m_ID);
	}

	void OpenGLVAO::LinkAttributes(VBO& VBO, uint32_t layout, uint32_t numComponents, ShaderDataType type, size_t stride, void* offset)
	{
		VBO.Bind();
		glEnableVertexAttribArray(layout);
		glVertexAttribPointer(layout, numComponents, getShaderDataTypeConvertOpenGL(type), GL_FALSE, stride, offset);
		VBO.Unbind();

		
	}

	void OpenGLVAO::LinkAttributes(VBO& VBO, uint32_t layout, ShaderDataType numComponents, ShaderDataType type, size_t stride, void* offset)
	{
		VBO.Bind();
		glEnableVertexAttribArray(layout);
		glVertexAttribPointer(layout, getShaderDataTypeComponent(numComponents), getShaderDataTypeConvertOpenGL(type), GL_FALSE, stride, offset);
		VBO.Unbind();
	}

	void OpenGLVAO::Bind() const
	{
		glBindVertexArray(m_ID);
	}

	void OpenGLVAO::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVAO::Delete()
	{
		glDeleteVertexArrays(1, &m_ID);
	}
}