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
		glBindVertexArray(m_ID);
	}

	void OpenGLVertexArray::LinkBuffers(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, VertexLayoutLinkInfo* vertexLayouts)
	{
		glBindVertexArray(m_ID);
		vertexBuffer->Bind();
		for (uint32_t i = 0; i < vertexLayouts->layoutCount; i++)
		{
			VertexLayout vertLayout = vertexLayouts->pLayouts[i];
			CHONPS_CORE_ASSERT(&vertLayout != nullptr, "Vertex Layout was nullptr!");
			glEnableVertexAttribArray(vertLayout.layout);
			glVertexAttribPointer(vertLayout.layout, vertLayout.numComponents, getShaderDataTypeConvertOpenGL(vertLayout.type), GL_FALSE, (GLsizei)vertLayout.stride, vertLayout.offset);
		}
		glBindVertexArray(0);
		vertexBuffer->Unbind();
		m_VertexCount = vertexBuffer->GetCount();
		m_IndexCount = indexBuffer->GetCount();
	}

	void OpenGLVertexArray::LinkBuffers(VertexArrayCreateInfo* createInfo)
	{
		glBindVertexArray(m_ID);
		createInfo->vertexBuffer->Bind();
		for (uint32_t i = 0; i < createInfo->vertexLayouts->layoutCount; i++)
		{
			VertexLayout vertLayout = createInfo->vertexLayouts->pLayouts[i];
			CHONPS_CORE_ASSERT(&vertLayout != nullptr, "Vertex Layout was nullptr!");
			glEnableVertexAttribArray(vertLayout.layout);
			glVertexAttribPointer(vertLayout.layout, vertLayout.numComponents, getShaderDataTypeConvertOpenGL(vertLayout.type), GL_FALSE, (GLsizei)vertLayout.stride, vertLayout.offset);
		}
		glBindVertexArray(0);
		createInfo->vertexBuffer->Unbind();
		m_VertexCount = createInfo->vertexBuffer->GetCount();
		m_IndexCount = createInfo->indexBuffer->GetCount();
	}

	void OpenGLVertexArray::Bind()
	{
		glBindVertexArray(m_ID);
	}

	void OpenGLVertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::Delete()
	{
		glDeleteVertexArrays(1, &m_ID);
	}
}