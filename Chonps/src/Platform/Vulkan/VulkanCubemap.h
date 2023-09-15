#ifndef HG_CHONPS_VULKAN_CUBEMAP_H
#define HG_CHONPS_VULKAN_CUBEMAP_H

#include "Graphics/Cubemap.h"
#include "VulkanRendererAPI.h"

namespace Chonps
{
	class VulkanCubemap : public Cubemap
	{
	public:
		VulkanCubemap(CubemapCreateInfo cubemapInfo, uint32_t setIndex);

		virtual void Bind(Shader* shader) override;
		virtual void Delete() override;

	private:
		void UpdateDescriptorSets(uint32_t currentImage);

		VkImage m_CubemapImage;
		VmaAllocation m_CubemapImageMemory = VK_NULL_HANDLE;
		VkImageView m_CubemapImageView;
		VkSampler m_CubemapSampler;

		VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
		VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
		VmaAllocation m_VertexBufferMemory = VK_NULL_HANDLE;
		VmaAllocation m_IndexBufferMemory = VK_NULL_HANDLE;

		std::vector<VkDescriptorSet> m_DescriptorSets;
		uint32_t m_SetIndex;
	};
}

#endif