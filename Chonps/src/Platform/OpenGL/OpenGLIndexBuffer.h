#ifndef OPENGL_INDEX_BUFFER_H
#define OPENGL_INDEX_BUFFER_H

#include "Graphics/IndexBuffer.h"

namespace Chonps
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		// IBO constructor for static arrays
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		// IBO constructor for dynamic arrays
		OpenGLIndexBuffer(std::vector<uint32_t>& indices);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetCount() const override { return m_Count; }
		virtual uint32_t* GetIndices() const override { return m_Indices; }
		virtual uint32_t GetSize() const override { return m_Count * sizeof(uint32_t); }

	private:
		uint32_t m_ID;
		uint32_t m_Count;
		uint32_t* m_Indices;
	};
}

#endif