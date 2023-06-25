#ifndef VULKAN_INDEX_BUFFER_H
#define VULKAN_INDEX_BUFFER_H

#include "Graphics/IndexBuffer.h"
#include "VulkanVertexBuffer.h"
#include <vulkan/vulkan.h>

namespace Chonps
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		// IBO constructor for static arrays
		VulkanIndexBuffer(uint32_t* indices, uint32_t size);
		// IBO constructor for dynamic arrays
		VulkanIndexBuffer(std::vector<uint32_t>& indices);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetCount() const override { return m_Count; }
		virtual uint32_t* GetIndices() const override { return m_Indices; }
		virtual uint32_t GetSize() const override { return m_Count * sizeof(uint32_t); }

	private:
		uint32_t m_Count;
		uint32_t* m_Indices;

		VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
		VkBuffer m_StagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_IndexBufferMemory = VK_NULL_HANDLE;
		VkDeviceMemory m_StagingBufferMemory = VK_NULL_HANDLE;
		VkDeviceSize m_IndexBufferSize;
	};
}

#endif