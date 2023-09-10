#ifndef HG_CHONPS_OPENGL_VERTEX_ARRAY_H
#define HG_CHONPS_OPENGL_VERTEX_ARRAY_H

#include "Graphics/VertexArray.h"

namespace Chonps
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();

		virtual void LinkBuffers(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, VertexLayoutLinkInfo* vertexLayouts) override;
		virtual void LinkBuffers(VertexArrayCreateInfo* createInfo) override;

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Delete() override;

		virtual uint32_t GetIndexCount() const override { return m_IndexCount; }
		virtual uint32_t GetVertexCount() const override { return m_VertexCount; }

		virtual uint32_t id() const override { return m_ID; }

	private:
		uint32_t m_ID;
		uint32_t m_IndexCount = 0;
		uint32_t m_VertexCount = 0;
	};
}

#endif