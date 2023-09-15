#ifndef HG_CHONPS_VULKAN_RENDERER_API_H
#define HG_CHONPS_VULKAN_RENDERER_API_H

#include "Graphics/RendererAPI.h" 
#include "Graphics/DrawList.h"
#include "Graphics/Pipeline.h"

#include "VulkanRendererBackends.h"

namespace Chonps
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual ~VulkanRendererAPI();
		std::shared_ptr<VulkanBackends> GetVulkanBackends() { return m_VulkanBackends; }

		virtual void Init() override;
		virtual void ClearColor(const float r, const float g, const float b, const float w) override;
		virtual void Clear() override;

		virtual void Draw(uint32_t vertexCount) override;
		virtual void DrawIndexed(uint32_t indexCount) override;
		virtual void DrawIndexed(VertexArray* vertexArray) override;
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance) override;
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance) override;

		virtual void SetStencilReference(uint32_t reference) override;
		virtual void SetStencilMask(uint32_t compareMask, uint32_t writeMask) override;

		virtual void BeginNextFrame() override;
		virtual void DrawSubmit() override;
		virtual void RenderPassBegin() override;
		virtual void RenderPassEnd() override;
		virtual void PushConstant(uint32_t size, uint32_t offset, ShaderStage shaderStage, const void* pValues) override;

		virtual void BindBufferSet(Shader* shader, UniformBuffer* buffer, uint32_t setIndex) override;

	private:
		void CreateInstance();
		bool SetUpDebugMessenger();
		void CreateSurface();
		void SetPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateDescriptorPool();
		void CreateCommandPool();
		void CreateDepthResources();
		void CreateFramebuffers();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void ThreadRender(uint32_t threadIndex, uint32_t cmdBufferIndex, VkCommandBufferInheritanceInfo inheritanceInfo);

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t count);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		void CreateOrUpdateTextureImage(uint32_t currentImage);

		void RecreateSwapChain();
		void CleanupSwapChain();


		DrawList m_DrawList;
		ThreadPool m_ThreadPool;
		std::shared_ptr<VulkanBackends> m_VulkanBackends;
	};									  

	namespace vks
	{
		VkVertexInputBindingDescription getBindingDescription(uint32_t stride);

		VkVertexInputAttributeDescription getAttributeDescriptions(uint32_t layout, VkFormat type, uint32_t stride, void* offset);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void createBuffer(VkDeviceSize size, VkBuffer& buffer, VmaAllocation& bufferMemory, VkBufferUsageFlags usage, VmaMemoryUsage memUsage);

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkAccessFlags srcMask, VkAccessFlags dstMask);

		VkCommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount = 1);

		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount = 1);

		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

		VulkanTextureData createSingleDataTexture(uint32_t width, uint32_t height, void* data);

		void fillQueueCountIDs(uint32_t firstElement, uint32_t finalElement);

		VkFormat findDepthFormat();

		bool hasStencilComponent(VkFormat format);

		void vkImplTextureBinding(uint32_t textureBinding, uint32_t samplerBinding, uint32_t frameBufferBinding, uint32_t cubemapBinding);

		VkSampleCountFlagBits getMaxUsableSampleCount();

		VkSampleCountFlagBits getSampleCountFlagBits(Sample samples);

		uint32_t getSampleCountValue(VkSampleCountFlagBits samples);

		VkSampleCountFlagBits getSupportedSampleCount(Sample samples);

		VkPrimitiveTopology getVulkanTopologyType(RenderTopologyType topologyType);

		VkCullModeFlags getVulkanCullModeFlag(RenderCullFaceMode cullFaceMode);

		VkFrontFace getVulkanCullFrontFace(RenderCullFrontFace cullFrontFace);

		VkColorComponentFlags getColorComponents(ColorWriteMask colorMask);

		VkBlendFactor getBlendFactor(ColorBlendFactor blendFactor);

		VkBlendOp getBlendOperation(ColorBlendOperation blendOp);

		VkCompareOp getCompareOp(CompareOperation compare);

		VkStencilOp getStencilOp(StencilOperation stencilOp);
	}
}

#endif