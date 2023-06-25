#include "cepch.h"
#include "VulkanVertexBuffer.h"

#include <vulkan/vulkan.h>

namespace Chonps
{
	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
		: VertexBuffer(vertices, size), m_Count(size / sizeof(float)), m_Vertices(vertices), m_Size(size)
	{
		VkDeviceSize bufferSize = size;
		m_VertexBufferSize = bufferSize;

		vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingBuffer, m_StagingBufferMemory);

		VkDevice device = getVulkanBackends()->device;
		void* data;
		vkMapMemory(device, m_StagingBufferMemory, 0, bufferSize, 0, &data);
		std::memcpy(data, vertices, (size_t)bufferSize);
		vkUnmapMemory(device, m_StagingBufferMemory);

		vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);
		vkSpec::copyBuffer(m_StagingBuffer, m_VertexBuffer, bufferSize);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(std::vector<vertex>& vertices)
		: VertexBuffer(vertices), m_Count(static_cast<uint32_t>(vertices.size())), m_Vertices((float*)vertices.data()), m_Size(static_cast<uint32_t>(sizeof(vertex) * vertices.size()))
	{
		VkDeviceSize bufferSize = sizeof(vertex) * vertices.size();
		m_VertexBufferSize = bufferSize;
		
		vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingBuffer, m_StagingBufferMemory);

		VkDevice device = getVulkanBackends()->device;
		void* data;
		vkMapMemory(device, m_StagingBufferMemory, 0, bufferSize, 0, &data);
		std::memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(device, m_StagingBufferMemory);

		vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);
		vkSpec::copyBuffer(m_StagingBuffer, m_VertexBuffer, bufferSize);
	}

	void VulkanVertexBuffer::Bind() const
	{
		getVulkanBackends()->vertexBuffer = m_VertexBuffer;
		getVulkanBackends()->stagingVertexBuffer = m_StagingBuffer;
		getVulkanBackends()->vertexBufferSize = m_VertexBufferSize;
	}

	void VulkanVertexBuffer::Unbind() const
	{
		getVulkanBackends()->vertexBuffer = VK_NULL_HANDLE;
		getVulkanBackends()->stagingVertexBuffer = VK_NULL_HANDLE;
		getVulkanBackends()->vertexBufferSize = 0;
	}

	void VulkanVertexBuffer::Delete()
	{
		VkDevice device = getVulkanBackends()->device;

		vkDestroyBuffer(device, m_VertexBuffer, nullptr);
		vkDestroyBuffer(device, m_StagingBuffer, nullptr);
		vkFreeMemory(device, m_VertexBufferMemory, nullptr);
		vkFreeMemory(device, m_StagingBufferMemory, nullptr);
	}
}