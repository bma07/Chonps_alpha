#include "cepch.h"
#include "VulkanIndexBuffer.h"

namespace Chonps
{
	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t size)
		: IndexBuffer(indices, size), m_Count(size / sizeof(uint32_t))
	{
		VkDeviceSize bufferSize = size;
		m_IndexBufferSize = bufferSize;

		vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingBuffer, m_StagingBufferMemory);

		VkDevice device = getVulkanBackends()->device;
		void* data;
		vkMapMemory(device, m_StagingBufferMemory, 0, bufferSize, 0, &data);
		std::memcpy(data, indices, (size_t)bufferSize);
		vkUnmapMemory(device, m_StagingBufferMemory);

		vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);
		vkSpec::copyBuffer(m_StagingBuffer, m_IndexBuffer, bufferSize);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(std::vector<uint32_t>& indices)
		: IndexBuffer(indices), m_Count(indices.size())
	{
		VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();
		m_IndexBufferSize = bufferSize;

		vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingBuffer, m_StagingBufferMemory);

		VkDevice device = getVulkanBackends()->device;
		void* data;
		vkMapMemory(device, m_StagingBufferMemory, 0, bufferSize, 0, &data);
		std::memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(device, m_StagingBufferMemory);

		vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);
		vkSpec::copyBuffer(m_StagingBuffer, m_IndexBuffer, bufferSize);
	}

	void VulkanIndexBuffer::Bind() const
	{
		getVulkanBackends()->indexBuffer = m_IndexBuffer;
		getVulkanBackends()->stagingIndexBuffer = m_StagingBuffer;
		getVulkanBackends()->indexBufferSize = m_IndexBufferSize;
	}

	void VulkanIndexBuffer::Unbind() const
	{
		getVulkanBackends()->indexBuffer = VK_NULL_HANDLE;
		getVulkanBackends()->stagingIndexBuffer = VK_NULL_HANDLE;
		getVulkanBackends()->indexBufferSize = 0;
	}

	void VulkanIndexBuffer::Delete()
	{
		VkDevice device = getVulkanBackends()->device;

		vkDestroyBuffer(device, m_IndexBuffer, nullptr);
		vkDestroyBuffer(device, m_StagingBuffer, nullptr);
		vkFreeMemory(device, m_IndexBufferMemory, nullptr);
		vkFreeMemory(device, m_StagingBufferMemory, nullptr);
	}
}