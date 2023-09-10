#ifndef HG_CHONPS_OPENGL_UNIFORM_BUFFER_H
#define HG_CHONPS_OPENGL_UNIFORM_BUFFER_H

#include "Graphics/UniformBuffer.h"

namespace Chonps
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t binding, uint32_t size, BufferType bufferType);

		virtual void Bind(const void* data, uint32_t size, uint32_t offset) override;
		virtual void Delete() override;

		virtual uint32_t id() override { return m_ID; }
	private:
		uint32_t m_ID;
		BufferType m_Type;
	};
}

#endif