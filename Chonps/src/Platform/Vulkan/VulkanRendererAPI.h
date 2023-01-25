#ifndef VULKAN_RENDERER_API_H
#define VULKAN_RENDERER_API_H

#include "Graphics/RendererAPI.h" 
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

	struct VulkanBackends
	{
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSurfaceKHR surface;

		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		VkRenderPass renderPass;
		VkVertexInputBindingDescription2EXT bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription2EXT> attributeDescriptions;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
		VkBuffer vertexBuffer;
		VkBuffer indexBuffer;
		VkBuffer stagingVertexBuffer;
		VkBuffer stagingIndexBuffer;
		VkDeviceSize vertexBufferSize;
		VkDeviceSize indexBufferSize;
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };

		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		bool framebufferResized = false;

		uint32_t currentFrame = 0;
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
		virtual void DrawLine() override;
		virtual void FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height);

		virtual void EnableCullFace() override;
		virtual void DisableCullFace() override;

		virtual void UploadUniform1f(uint32_t shader, const char* uniform, float x) override;
		virtual void UploadUniform2f(uint32_t shader, const char* uniform, float x, float y) override;
		virtual void UploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z) override;
		virtual void UploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w) override;

		virtual void UploadUniform1i(uint32_t shader, const char* uniform, int x) override;
		virtual void UploadUniform2i(uint32_t shader, const char* uniform, int x, int y) override;
		virtual void UploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z) override;
		virtual void UploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w) override;

		virtual void UploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x) override;
		virtual void UploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y) override;
		virtual void UploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z) override;
		virtual void UploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w) override;

		virtual void UploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) override;
		virtual void UploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) override;
		virtual void UploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) override;
		virtual void UploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v) override;

		virtual void UploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) override;
		virtual void UploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) override;
		virtual void UploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) override;
		virtual void UploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v) override;

		virtual void UploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) override;
		virtual void UploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) override;
		virtual void UploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) override;
		virtual void UploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v) override;

		virtual void UploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;

		virtual void UploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;
		virtual void UploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v) override;

	private:
		void CreateInstance();
		bool SetUpDebugMessenger();
		void CreateSurface();
		void SetPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateGraphicsPipeline();
		void CreateFramebuffers();
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const uint32_t& count);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		
		void RecreateSwapChain();
		void CleanupSwapChain();

		std::shared_ptr<VulkanBackends> m_VulkanBackends;
	};									  

	void setCurrentWindowForVulkanWindowSurface(GLFWwindow* window);
	VulkanBackends* getVulkanBackends();
}

#endif