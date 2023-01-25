#ifndef VULKAN_VERTEX_ARRAY_H
#define VULKAN_VERTEX_ARRAY_H

#include "Graphics/VertexArray.h"
#include <vulkan/vulkan.h>

namespace Chonps
{
	class VulkanVertexArray : public VertexArray
	{
	public:
		VulkanVertexArray();

		virtual void LinkVertexBuffer(VertexBuffer* VBO, uint32_t layout, uint32_t numComponents, ShaderDataType type, uint32_t stride, void* offset) override;
		virtual void LinkIndexBuffer(IndexBuffer* IBO) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetIndexCount() const override { return m_IndexCount; }
		virtual uint32_t GetVertexCount() const override { return m_VertexCount; }

	private:
		uint32_t m_IndexCount = 0;
		uint32_t m_VertexCount = 0;

		std::vector<VkVertexInputAttributeDescription2EXT> m_AttributeDescriptions;

		VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
		VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;
		VkDeviceMemory m_IndexBufferMemory = VK_NULL_HANDLE;

		bool m_FirstVertexInit = false;
		bool m_FirstIndexInit = false;
	};
}

#endif