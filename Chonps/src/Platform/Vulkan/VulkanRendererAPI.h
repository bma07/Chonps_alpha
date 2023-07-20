#ifndef VULKAN_RENDERER_API_H
#define VULKAN_RENDERER_API_H

#include "Graphics/RendererAPI.h" 
#include "Graphics/DrawList.h"
#include "Platform/WindowAPI/glfwAPI/glfwWindowAPI.h"

namespace Chonps
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool complete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct VulkanPipelineShaderStages
	{
		VkPipelineShaderStageCreateInfo vertexShaderStage;
		VkPipelineShaderStageCreateInfo fragementShaderStage;
	};

	struct VulkanTextureData
	{
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
		uint32_t unit;
	};

	struct PushConstantTextureIndexData
	{
		int texIndex[32];
	};

	struct VulkanPipeline
	{
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};

	struct ThreadDrawCmdData
	{
		DrawCommand drawCmd;
		VkBuffer vertexBuffer, indexBuffer;
	};

	struct ThreadData
	{
		std::vector<ThreadDrawCmdData> drawCommandData;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
	};

	struct VulkanBackends
	{
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		QueueFamilyIndices queueFamiliyIndices;
		VkSurfaceKHR surface;
		VkPhysicalDeviceProperties deviceProperties;

		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		const uint32_t maxFramesInFlight = 3;
		const uint32_t maxDescriptorCounts = 65536;
		uint64_t maxObjects = 10000;
		uint32_t maxTextures = 4096;
		std::queue<uint32_t> shaderCountIDs;

		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkVertexInputBindingDescription2EXT bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription2EXT> attributeDescriptions;
		std::vector<VkDescriptorSetLayout> toPipelineDescriptorSetLayouts;
		std::unordered_map<uint32_t, VkDescriptorSetLayout> uboDescriptorSetLayouts;
		std::unordered_map<uint32_t, VulkanPipeline> graphicsPipeline;
		std::unordered_map<uint32_t, VulkanPipelineShaderStages> pipelineShaderStages;
		uint32_t currentBindedGraphicsPipeline = 0;
		VkPushConstantRange pushConstantRange;
		uint32_t pushConstantRangeCount = 0;
		uint32_t imageIndex;

		VkBuffer bufferArray = VK_NULL_HANDLE;
		VkDeviceMemory bufferArrayMemory = VK_NULL_HANDLE;
		VkDeviceSize bufferArraySize = 0;
		VkDeviceSize bufferArrayIndexOffset = 0;

		VkBuffer vertexBuffer;
		VkBuffer indexBuffer;
		VkBuffer stagingVertexBuffer;
		VkBuffer stagingIndexBuffer;
		VkDeviceSize vertexBufferSize;
		VkDeviceSize indexBufferSize;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<void*> uniformBuffersMapped;
		uint32_t dynamicAlignment;
		uint32_t maxUniformBuffers = 4096;
		VkDescriptorPool descriptorPool;
		uint32_t currentDescriptorSetBinding;
		std::vector<VkDescriptorSet> descriptorSets;
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkDescriptorSet> samplerDescriptorSet;
		VkDescriptorSetLayout samplerDescriptorSetsLayout;
		std::vector<VkDescriptorSet> nullSamplerDescriptorSets;
		uint32_t drawCallCount = 0, drawCallCountTotal = 0;
		bool firstTimeDrawCallCount = true;

		const void* uboData;
		size_t uboSize;
		uint32_t uboOffset;

		uint32_t textureBinding, samplerBinding;
		std::queue<uint32_t> textureCountIDs;
		std::vector<std::queue<uint32_t>> texturesQueue;
		std::unordered_map<uint32_t, VulkanTextureData> textureImages;
		std::vector<VkDescriptorImageInfo> imageInfos;
		std::unordered_set<uint32_t> currentBindedTextureImages;
		PushConstantTextureIndexData textureIndexConstant;
		VulkanTextureData nullDataTexture;
		bool allowClearBindedTextures = false;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		VkClearValue clearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};

		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		bool framebufferResized = false;
		uint32_t currentFrame = 0;

		bool multithreadingEnabled = true;
		uint32_t numThreads;
		uint32_t threadIndex = 0;
	};

	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual ~VulkanRendererAPI();
		std::shared_ptr<VulkanBackends> GetVulkanBackends() { return m_VulkanBackends; }

		virtual void Init() override;
		virtual void ClearColor(const float r, const float g, const float b, const float w) override;
		virtual void Clear() override;

		virtual void Draw(const uint32_t& count) override;
		virtual void Draw(VertexArray* vertexArray) override;

		virtual void DrawLine(VertexArray* vertexArray) override;

		virtual void BeginNextFrame() override;
		virtual void DrawSubmit() override;

		virtual void FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height) override;

		virtual void EnableCullFace() override;
		virtual void DisableCullFace() override;

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

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const uint32_t& count);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		void CreateOrUpdateUniformBuffer(uint32_t currentImage, uint32_t currentDrawCall);
		void CreateOrUpdateTextureImage(uint32_t currentImage);

		void RecreateSwapChain();
		void CleanupSwapChain();


		PFN_vkCmdSetVertexInputEXT CmdSetVertexInputEXT{};

		DrawList m_DrawList;
		ThreadPool m_ThreadPool;
		std::vector<ThreadData> m_ThreadData;
		std::shared_ptr<VulkanBackends> m_VulkanBackends;
	};									  

	void setCurrentWindowForVulkanWindowSurface(GLFWwindow* window);
	VulkanBackends* getVulkanBackends();

	namespace vkSpec
	{
		VkVertexInputBindingDescription2EXT getBindingDescription(uint32_t stride);

		VkVertexInputAttributeDescription2EXT getAttributeDescriptions(uint32_t layout, VkFormat type, uint32_t stride, void* offset);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		VkCommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

		VulkanTextureData createSingleDataTexture(uint32_t width, uint32_t height, void* data);

		void fillQueueCountIDs();

		void vkImplPrepareDraw();
	}
}

#endif