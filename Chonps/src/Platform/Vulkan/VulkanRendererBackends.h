#ifndef HG_CHONPS_VULKAN_RENDERER_BACKENDS_H
#define HG_CHONPS_VULKAN_RENDERER_BACKENDS_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Core/QueueChain.h"
#include "Graphics/VertexBuffer.h"
#include "Platform/WindowAPI/glfwAPI/glfwWindowAPI.h"

namespace Chonps
{
	struct TextureDescriptorSetBindings
	{
		uint32_t textureBinding = 0, samplerBinding = 1, frameBufferBinding = 0, cubemapBinding = 0;
	};

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

	struct VulkanPipeline
	{
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};

	struct VulkanPipelineLayoutData
	{
		VkDescriptorSetLayout* pDescrptorSetLayouts;
		uint32_t descriptorSetLayoutCount;
		VkPushConstantRange* pPushConstants;
		uint32_t pushConstantCount;
		VkRenderPass renderPass;
	};

	struct VulkanPipelineShaderStages
	{
		VkPipelineShaderStageCreateInfo vertexShaderStage;
		VkPipelineShaderStageCreateInfo fragementShaderStage;
	};

	struct VulkanBufferData
	{
		VkBuffer buffer, staging;
		VkDeviceSize size;
		BufferState state;
	};

	struct VulkanTextureData
	{
		VkImage textureImage;
		VmaAllocation textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
		uint32_t unit;
	};

	struct PushConstantTextureIndexData
	{
		int texIndex[32];
	};

	struct VulkanFrameBufferAttachment
	{
		VkImage image = VK_NULL_HANDLE;
		VmaAllocation memory = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
		VkFormat format;
	};

	struct VulkanFrameBufferData
	{
		VkFramebuffer framebuffer;
		std::vector<VulkanFrameBufferAttachment> color;
		std::vector<VulkanFrameBufferAttachment> msaaColor;
		VulkanFrameBufferAttachment depth;
		VkDescriptorImageInfo descriptor;
	};

	struct OffscreenPass
	{
		int width, height;
		VkRenderPass renderPass;
		VkSampler sampler;
		VulkanFrameBufferData framebuffer;
		std::vector<VkCommandBuffer> cmdBuffers;
	};

	struct VulkanUniformBufferData
	{
		std::vector<VkBuffer> buffers;
		std::vector<VmaAllocation> buffersMemory;
		std::vector<void*> buffersMapped;
		VkDeviceSize bufferSize;
		VkDescriptorType bufferType;
	};

	struct VulkanBackends
	{
		bool enableValidationLayers = false;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		QueueFamilyIndices queueFamiliyIndices;
		VkSurfaceKHR surface;
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		int windowWidth, windowHeight;
		GLFWwindow* currentWindow;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		uint32_t swapChainCount;
		uint32_t maxFramesInFlight = 3;
		uint32_t maxDescriptorCounts = 65536;
		uint32_t maxFrameBufferColorAttachments = 8;
		size_t maxObjects = 0x80000;
		uint32_t maxTextureBindingSlots = 32;
		uint32_t maxObjectIDs = 0xffff;
		QueueChain<uint32_t> uniformBufferCountIDs;
		QueueChain<uint32_t> shaderCountIDs;
		QueueChain<uint32_t> vertexBufferCountIDs;
		QueueChain<uint32_t> indexBufferCountIDs;
		QueueChain<uint32_t> vertexArrayBufferCountIDs;
		QueueChain<uint32_t> textureCountIDs;
		QueueChain<uint32_t> frameBufferCountIDs;

		VkRenderPass renderPass;
		std::unordered_map<uint32_t, VkRenderPass> frameBufferRenderPasses;
		VkRenderPassBeginInfo renderPassInfo;
		std::unordered_map<uint32_t, VulkanPipeline> graphicsPipeline;
		uint32_t currentBindedGraphicsPipeline = 0;
		uint32_t imageIndex;

		VkBuffer bufferArray = VK_NULL_HANDLE;
		VkDeviceMemory bufferArrayMemory = VK_NULL_HANDLE;
		VkDeviceSize bufferArraySize = 0;
		VkDeviceSize bufferArrayIndexOffset = 0;


		std::unordered_map<uint32_t, VulkanUniformBufferData*> uniformBuffersData;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<void*> uniformBuffersMapped;
		uint32_t dynamicAlignment;
		uint32_t maxUniformBuffers = 4096;
		VkDescriptorPool descriptorPool;
		uint32_t currentDescriptorSetBinding;
		std::unordered_map<uint32_t, std::vector<VkDescriptorSet>> bufferDescriptorSets;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		std::vector<std::pair<uint32_t, VkDescriptorSetLayout>> descriptorSetLayoutIndex;

		VkDescriptorSetLayout textureDescriptorSetLayout, frameBufferDescriptorSetLayout, cubemapDescriptorSetLayout;
		uint32_t drawCallCount = 0, drawCallCountTotal = 0;

		const void* uboData;
		size_t uboSize;
		uint32_t uboOffset;

		TextureDescriptorSetBindings* textureDescriptorBindings;
		std::vector<std::queue<uint32_t>> texturesQueue;
		std::unordered_map<uint32_t, VulkanTextureData> textureImages;
		std::vector<VkDescriptorImageInfo> nullImageInfos;
		VulkanTextureData nullDataTexture;


		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		VkClearValue clearColor = { {0.0f, 0.0f, 0.0f, 1.0f} };

		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		bool framebufferResized = false;
		uint32_t currentFrame = 0;

		bool multithreadingEnabled = false;
		uint32_t numThreads;
		uint32_t threadIndex = 0;
	};

	void setCurrentWindowForVulkanWindowSurface(GLFWwindow* window);
	VulkanBackends* getVulkanBackends();
	VmaAllocator& getVmaAllocator();
}

#endif