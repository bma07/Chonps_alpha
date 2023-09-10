#ifndef HG_CHONPS_VULKAN_VERTEX_BUFFER_H
#define HG_CHONPS_VULKAN_VERTEX_BUFFER_H

#include "VulkanRendererAPI.h"
#include "Graphics/VertexBuffer.h"
#include "Core/DataTypes.h"

namespace Chonps
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		// VBO constructor for static arrays
		VulkanVertexBuffer(float* vertices, uint32_t size, BufferState bufferState = BufferState::Static);
		// VBO constructor for dynamic array
		VulkanVertexBuffer(std::vector<vertex>& vertices, BufferState bufferState = BufferState::Static);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetCount() const override { return m_Count; }
		virtual float* GetVertices() const override { return m_Vertices; }
		virtual uint32_t GetSize() const override { return m_Size; }

		virtual void Subdata(float* vertices, uint32_t size) override;
		virtual void Subdata(std::vector<vertex>& vertices) override;

		virtual uint32_t id() const override { return m_ID; }
		VulkanBufferData& getBufferData() { return m_BufferData; }

	private:
		uint32_t m_ID;
		VulkanBufferData m_BufferData;
		bool m_UseStaging = true;

		uint32_t m_Count;
		float* m_Vertices;
		uint32_t m_Size;
		VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
		VkBuffer m_StagingBuffer = VK_NULL_HANDLE;
		VmaAllocation m_VertexBufferMemory = VK_NULL_HANDLE;
		VmaAllocation m_StagingBufferMemory = VK_NULL_HANDLE;
		VkDeviceSize m_VertexBufferSize;
	};
}

#endif