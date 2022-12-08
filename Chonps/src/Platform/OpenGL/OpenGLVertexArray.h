#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H

#include "Graphics/VertexArray.h"

namespace Chonps
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();

		virtual void LinkVertexBuffer(VertexBuffer* VBO, uint32_t layout, uint32_t numComponents, ShaderDataType type, uint32_t stride, void* offset) override;
		virtual void LinkVertexBuffer(VertexBuffer* VBO, uint32_t layout, ShaderDataType numComponents, ShaderDataType type, uint32_t stride, void* offset) override;
		
		virtual void LinkIndexBuffer(IndexBuffer* IBO) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetIndexCount() const override { return m_IndexCount; }
		virtual uint32_t GetVertexCount() const override { return m_VertexCount; }

	private:
		uint32_t m_ID;
		uint32_t m_IndexCount = 0;
		uint32_t m_VertexCount = 0;
	};
}

#endif