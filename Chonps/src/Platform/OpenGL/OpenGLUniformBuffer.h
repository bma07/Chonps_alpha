#ifndef OPENGL_UNIFORM_BUFFER_H
#define OPENGL_UNIFORM_BUFFER_H

#include "Graphics/UniformBuffer.h"

namespace Chonps
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t binding, uint32_t size);

		virtual void Bind(const void* data, uint32_t size, uint32_t offset) override;
		virtual void Delete() override;

	private:
		uint32_t m_ID;
	};
}

#endif