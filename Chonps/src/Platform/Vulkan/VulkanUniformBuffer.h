#ifndef VULKAN_UNIFORM_BUFFER_H
#define VULKAN_UNIFORM_BUFFER_H

#include "Graphics/UniformBuffer.h"

#include <vulkan/vulkan.h>

namespace Chonps
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t binding, uint32_t size);

		virtual void Bind(const void* data, uint32_t size, uint32_t offset) override;
		virtual void Delete() override;

	private:
		VkDescriptorSetLayout m_DescriptorSetLayout;
		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_UniformBuffersMapped;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		uint32_t m_Binding;
	};
}

#endif