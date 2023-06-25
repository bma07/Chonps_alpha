#ifndef CHONPS_UNIFORM_BUFFER_H
#define CHONPS_UNIFORM_BUFFER_H

namespace Chonps
{
	class UniformBuffer
	{
	public:
		UniformBuffer(uint32_t binding, uint32_t size) {}

		virtual void Bind(const void* data, uint32_t size, uint32_t offset) = 0;
		virtual void Delete() = 0;
	};

	std::shared_ptr<UniformBuffer> createUniformBufferSp(uint32_t binding, uint32_t size);
	UniformBuffer* createUniformBuffer(uint32_t binding, uint32_t size);
}

#endif