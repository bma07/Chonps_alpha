#ifndef HG_CHONPS_VULKAN_UNIFORM_BUFFER_H
#define HG_CHONPS_VULKAN_UNIFORM_BUFFER_H

#include "Graphics/UniformBuffer.h"
#include "VulkanRendererAPI.h"

namespace Chonps
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t binding, uint32_t size, BufferType bufferType);

		virtual void Bind(const void* data, uint32_t size, uint32_t offset) override;
		virtual void Delete() override;

		virtual uint32_t id() override { return m_ID; }

	private:

		VulkanUniformBufferData m_BufferData;
		uint32_t m_Binding, m_ID;
	};

	namespace vks
	{
		BufferLayout vkImplCreateBufferLayout(BufferBindingCreateLayoutsInfo* bufferBindings, uint32_t setIndex);
	}
}

#endif