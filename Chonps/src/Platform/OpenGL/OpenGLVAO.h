#ifndef OPENGL_VAO_H
#define OPENGL_VAO_H

#include "Graphics/VAO.h"

namespace Chonps
{
	class OpenGLVAO : public VAO
	{
	public:
		OpenGLVAO();

		virtual void LinkAttributes(VBO& VBO, uint32_t layout, uint32_t numComponents, ShaderDataType type, size_t stride, void* offset) override;
		virtual void LinkAttributes(VBO& VBO, uint32_t layout, ShaderDataType numComponents, ShaderDataType type, size_t stride, void* offset) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

	private:
		uint32_t m_ID;
	};
}

#endif