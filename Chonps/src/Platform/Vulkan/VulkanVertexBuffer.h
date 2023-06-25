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
		virtual float* GetVertices() const override { return m_Vertices; }
		virtual uint32_t GetSize() const override { return m_Size; }

	private:
		uint32_t m_Count;
		float* m_Vertices;
		uint32_t m_Size;
		VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
		VkBuffer m_StagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;
		VkDeviceMemory m_StagingBufferMemory = VK_NULL_HANDLE;
		VkDeviceSize m_VertexBufferSize;
	};
}

#endif