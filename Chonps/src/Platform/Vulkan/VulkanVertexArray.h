#ifndef HG_CHONPS_VULKAN_VERTEX_ARRAY_H
#define HG_CHONPS_VULKAN_VERTEX_ARRAY_H

#include "Graphics/VertexArray.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanRendererAPI.h"

namespace Chonps
{
	class VulkanVertexArray : public VertexArray
	{
	public:
		VulkanVertexArray();
		
		virtual void LinkBuffers(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, VertexLayoutLinkInfo* vertexLayouts) override;
		virtual void LinkBuffers(VertexArrayCreateInfo* createInfo) override;

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Delete() override;

		virtual uint32_t GetIndexCount() const override { return m_IndexCount; }
		virtual uint32_t GetVertexCount() const override { return m_VertexCount; }

		virtual uint32_t id() const override { return m_ID; }

	private:
		uint32_t m_ID;
		bool m_StateStatic = true;

		uint32_t m_IndexCount = 0, m_VertexCount = 0;
		VkDeviceSize m_VertexBufferSize = 0, m_IndexBufferSize = 0, m_VertexArrayBufferSize = 0;
		std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;

		VkBuffer m_VertexBuffer = VK_NULL_HANDLE, m_IndexBuffer = VK_NULL_HANDLE;
		VkBuffer m_VertexArrayBuffer = VK_NULL_HANDLE;
		VmaAllocation m_VertexArrayBufferMemory = VK_NULL_HANDLE;
	};

	namespace vks
	{
		VkFormat getShaderDataTypeConvertVulkan(ShaderDataType type, uint32_t numComponents);
	}
}

#endif