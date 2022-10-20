#ifndef FBO_H
#define FBO_H

namespace Chonps
{
	class FBO
	{
	public:
		FBO(unsigned int width, unsigned int height) {}

		virtual void Submit() = 0;
		virtual void UpdateFrameBuffer(unsigned int width, unsigned int height) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetTexID() const = 0;
	};

	FBO* createFBO(unsigned int width, unsigned int height);
}

#endif