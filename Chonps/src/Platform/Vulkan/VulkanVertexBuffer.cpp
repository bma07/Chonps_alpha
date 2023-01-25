#include "cepch.h"
#include "VulkanVertexBuffer.h"

#include <vulkan/vulkan.h>

namespace Chonps
{
	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
		: VertexBuffer(vertices, size), m_Count(size / sizeof(float))
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
		: VertexBuffer(vertices), m_Count(vertices.size())
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

	namespace vkSpec
	{
		static uint32_t s_Stride = sizeof(vertex);
		
		VkVertexInputBindingDescription2EXT getBindingDescription()
		{
			VkVertexInputBindingDescription2EXT bindingDescription{};
			bindingDescription.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
			bindingDescription.binding = 0;
			bindingDescription.divisor = 1;
			bindingDescription.stride = s_Stride;
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; 
			return bindingDescription;
		}

		VkVertexInputAttributeDescription2EXT getAttributeDescriptions(uint32_t layout, VkFormat type, uint32_t stride, void* offset)
		{
			VkVertexInputAttributeDescription2EXT attributeDescriptions{};
			attributeDescriptions.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
			attributeDescriptions.binding = 0;
			attributeDescriptions.location = layout;
			attributeDescriptions.format = type;
			attributeDescriptions.offset = static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(offset));
			s_Stride = stride;
			getVulkanBackends()->bindingDescriptions = getBindingDescription();
			
			return attributeDescriptions;
		}

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(getVulkanBackends()->physicalDevice, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return i;
			}

			CHONPS_CORE_ASSERT(false, "Failed to find suitable memory type!");
			return -1;
		}

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VkDevice device = getVulkanBackends()->device;

			CHONPS_CORE_ASSERT(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) == VK_SUCCESS, "Failed to create buffer!");

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

			CHONPS_CORE_ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) == VK_SUCCESS, "Failed to allocate buffer memory!");

			vkBindBufferMemory(device, buffer, bufferMemory, 0);
		}

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = getVulkanBackends()->commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(getVulkanBackends()->device, &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
		
			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(getVulkanBackends()->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(getVulkanBackends()->graphicsQueue);

			vkFreeCommandBuffers(getVulkanBackends()->device, getVulkanBackends()->commandPool, 1, &commandBuffer);
		}
	}
}