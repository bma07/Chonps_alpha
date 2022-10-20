#ifndef OPENGL_VBO_H
#define OPENGL_VBO_H

#include "Graphics/VBO.h"

namespace Chonps
{
	class OpenGLVBO : public VBO
	{
	public:
		// VBO constructor for static arrays
		OpenGLVBO(float* vertices, size_t size);
		// VBO constructor for dynamic array
		OpenGLVBO(std::vector<vertex>& vertices);


		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

	private:
		uint32_t m_ID;
	};
}

#endif