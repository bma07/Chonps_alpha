#ifndef HG_CHONPS_VULKAN_INDEX_BUFFER_H
#define HG_CHONPS_VULKAN_INDEX_BUFFER_H

#include "Graphics/IndexBuffer.h"
#include "VulkanVertexBuffer.h"

namespace Chonps
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		// IBO constructor for static arrays
		VulkanIndexBuffer(uint32_t* indices, uint32_t size, BufferState bufferState = BufferState::Static);
		// IBO constructor for dynamic arrays
		VulkanIndexBuffer(std::vector<uint32_t>& indices, BufferState bufferState = BufferState::Static);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetCount() const override { return m_Count; }
		virtual uint32_t* GetIndices() const override { return m_Indices; }
		virtual uint32_t GetSize() const override { return m_Size; }

		virtual void Subdata(uint32_t* indices, uint32_t size) override;
		virtual void Subdata(std::vector<uint32_t>& indices) override;

		virtual uint32_t id() const override { return m_ID; }
		VulkanBufferData getBufferData() { return m_BufferData; }

	private:
		uint32_t m_ID;
		VulkanBufferData m_BufferData;
		bool m_UseStaging = true;

		uint32_t m_Count;
		uint32_t m_Size;
		uint32_t* m_Indices;

		VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
		VkBuffer m_StagingBuffer = VK_NULL_HANDLE;
		VmaAllocation m_IndexBufferMemory = VK_NULL_HANDLE;
		VmaAllocation m_StagingBufferMemory = VK_NULL_HANDLE;
		VkDeviceSize m_IndexBufferSize;
	};
}

#endif