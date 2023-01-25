#ifndef VULKAN_VERTEX_BUFFER_H
#define VULKAN_VERTEX_BUFFER_H

#include "VulkanRendererAPI.h"
#include "Graphics/VertexBuffer.h"
#include "Core/DataTypes.h"

namespace Chonps
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		// VBO constructor for static arrays
		VulkanVertexBuffer(float* vertices, uint32_t size);
		// VBO constructor for dynamic array
		VulkanVertexBuffer(std::vector<vertex>& vertices);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_Count;
		VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
		VkBuffer m_StagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;
		VkDeviceMemory m_StagingBufferMemory = VK_NULL_HANDLE;
		VkDeviceSize m_VertexBufferSize;
	};

	namespace vkSpec
	{
		VkVertexInputBindingDescription2EXT getBindingDescription();

		VkVertexInputAttributeDescription2EXT getAttributeDescriptions(uint32_t layout, VkFormat type, uint32_t stride, void* offset);
		
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	}
}

#endif