#ifndef OPENGL_EBO_H
#define OPENGL_EBO_H

#include "Graphics/EBO.h"

namespace Chonps
{
	class OpenGLEBO : public EBO
	{
	public:
		// EBO constructor for static arrays
		OpenGLEBO(uint32_t* indices, size_t size);
		// EBO constructor for dynamic arrays
		OpenGLEBO(std::vector<uint32_t>& indices);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_ID;
		uint32_t m_Count;
	};
}

#endif