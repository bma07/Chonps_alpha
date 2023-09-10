#ifndef HG_CHONPS_VULKAN_FRAMEBUFFER_H
#define HG_CHONPS_VULKAN_FRAMEBUFFER_H

#include "VulkanRendererAPI.h"
#include "Graphics/FrameBuffer.h"

namespace Chonps
{
	class VulkanFrameBuffer : public FrameBuffer
	{
	public:
		VulkanFrameBuffer(FrameBufferSpecificationInfo frameBufferSpecificationInfo);

		virtual void Draw(uint32_t index = 0) override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Begin() override;
		virtual void End() override;

		virtual void BindTex(uint32_t slot = 0) override;

		virtual void Delete() override;

		virtual uint32_t id() const override { return m_ID; }
		virtual uint32_t GetTexID(uint32_t index = 0) const override { return m_TexID; }

		virtual unsigned int GetWidth() override { return m_FrameBufferSpecificationInfo.width; }
		virtual unsigned int GetHeight() override { return m_FrameBufferSpecificationInfo.height; }

		VkRenderPass getRenderPass() { return m_OffscreenPass.renderPass; }

	private:
		OffscreenPass m_OffscreenPass;

		FrameBufferSpecificationInfo m_FrameBufferSpecificationInfo;
		std::vector<FrameBufferColorAttachment> m_ColorAttachments;
		FrameBufferDepthAttachment m_DepthAttachment = { 0, FrameBufferDepthFormat::None };
		bool m_DepthAttached = false;

		uint32_t m_ID, m_TexID, m_Unit = 0;
		uint32_t m_VPx, m_VPy, m_VPWidth, m_VPHeight;
		bool m_MultiSampled = false;

		VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
		VmaAllocation m_VertexBufferMemory = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> m_DescriptorSets;
		std::vector<VkClearValue> m_ClearValues;

		void CreateFrameBuffer(VkRenderPass renderPass, VulkanFrameBufferData* frameBuf, FrameBufferSpecificationInfo specification);
		void UpdateFrameBufferImage(uint32_t currentImage);
	};
}

#endif