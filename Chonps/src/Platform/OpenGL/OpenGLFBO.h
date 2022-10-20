#ifndef OPENGL_FBO_H
#define OPENGL_FBO_H

#include "Graphics/FBO.h"

namespace Chonps
{
	class OpenGLFBO : public FBO
	{
	public:
		OpenGLFBO(unsigned int width, unsigned int height);

		virtual void Submit() override;
		virtual void UpdateFrameBuffer(unsigned int width, unsigned int height) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetTexID() const override { return m_Texture; }

	private:
		unsigned int m_Width;
		unsigned int m_Height;

		uint32_t m_ID;
		uint32_t m_Texture;
		uint32_t m_RBO;
		uint32_t m_VAO;
	};
}

#endif