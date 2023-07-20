#include "cepch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanRendererAPI.h"

namespace Chonps
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t binding, uint32_t size)
		: UniformBuffer(binding, size), m_Binding(binding)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VkDeviceSize bufferSize = size * vkBackends->maxObjects;

		// Create Descriptor Layout
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = binding;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo uboLayoutInfo{};
		uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		uboLayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
		uboLayoutInfo.bindingCount = 1;
		uboLayoutInfo.pBindings = &uboLayoutBinding;
		
		CHONPS_CORE_ASSERT(vkCreateDescriptorSetLayout(vkBackends->device, &uboLayoutInfo, nullptr, &m_DescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");

		// Create Uniform Buffers
		m_UniformBuffers.resize(vkBackends->maxFramesInFlight);
		m_UniformBuffersMemory.resize(vkBackends->maxFramesInFlight);
		m_UniformBuffersMapped.resize(vkBackends->maxFramesInFlight);

		for (size_t i = 0; i < vkBackends->maxFramesInFlight; i++)
		{
			vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i], m_UniformBuffersMemory[i]);
			vkMapMemory(vkBackends->device, m_UniformBuffersMemory[i], 0, bufferSize, 0, &m_UniformBuffersMapped[i]);
		}

		// Create Descriptor Sets
		std::vector<VkDescriptorSetLayout> uboLayouts(vkBackends->maxFramesInFlight, m_DescriptorSetLayout);
		VkDescriptorSetAllocateInfo uboAllocInfo{};
		uboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		uboAllocInfo.descriptorPool = vkBackends->descriptorPool;
		uboAllocInfo.descriptorSetCount = static_cast<uint32_t>(uboLayouts.size());
		uboAllocInfo.pSetLayouts = uboLayouts.data();

		m_DescriptorSets.resize(vkBackends->maxFramesInFlight);
		CHONPS_CORE_ASSERT(vkAllocateDescriptorSets(vkBackends->device, &uboAllocInfo, m_DescriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");

		for (size_t i = 0; i < m_DescriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = bufferSize;

			VkWriteDescriptorSet descriptorWrites{};
			descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites.dstSet = m_DescriptorSets[i];
			descriptorWrites.dstBinding = binding;
			descriptorWrites.dstArrayElement = 0;
			descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites.descriptorCount = 1;
			descriptorWrites.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(vkBackends->device, 1, &descriptorWrites, 0, nullptr);
		}

		vkBackends->uboDescriptorSetLayouts[binding] = m_DescriptorSetLayout;
	}

	void VulkanUniformBuffer::Bind(const void* data, uint32_t size, uint32_t offset)
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		vkBackends->uniformBuffers = m_UniformBuffers;
		vkBackends->uniformBuffersMapped = m_UniformBuffersMapped;
		vkBackends->descriptorSets = m_DescriptorSets;
		vkBackends->descriptorSetLayout = m_DescriptorSetLayout;
		vkBackends->currentDescriptorSetBinding = m_Binding;

		vkBackends->uboData = data;
		vkBackends->uboSize = size;
		vkBackends->uboOffset = offset;
	}

	void VulkanUniformBuffer::Delete()
	{
		VkDevice device = getVulkanBackends()->device;

		for (size_t i = 0; i < getVulkanBackends()->maxFramesInFlight; i++)
		{
			vkDestroyBuffer(device, m_UniformBuffers[i], nullptr);
			vkFreeMemory(device, m_UniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
	}
}