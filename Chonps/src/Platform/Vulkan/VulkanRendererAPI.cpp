#include "cepch.h"
#include "VulkanRendererAPI.h"

#include "Core/File.h"
#include "VulkanPipeline.h"
#include "VulkanVertexBuffer.h"

#include <optional>
#include <future>

const std::vector<const char*> validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_MAINTENANCE3_EXTENSION_NAME,
	VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
	VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
};

#ifdef CHONPS_DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif



namespace Chonps
{
	static GLFWwindow* s_CurrentWindow;
	static std::shared_ptr<VulkanBackends> s_VulkanBackends;

	void setCurrentWindowForVulkanWindowSurface(GLFWwindow* window)
	{
		s_CurrentWindow = window;
	}

	VulkanBackends* getVulkanBackends()
	{
		return &(*s_VulkanBackends);
	}

	namespace vkUtils
	{
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
		{
			switch (messageSeverity)
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			{
				CHONPS_CORE_INFO("VULKAN: Validation Layer: {0}", pCallbackData->pMessage);
				break;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			{
				CHONPS_CORE_WARN("VULKAN: Validation Layer: {0}", pCallbackData->pMessage);
				break;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			{
				CHONPS_CORE_ERROR("VULKAN: Validation Layer: {0}", pCallbackData->pMessage);
				break;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			{
				CHONPS_CORE_ERROR("VULKAN: Validation Layer: {0}", pCallbackData->pMessage);
				break;
			}
			}

			return VK_FALSE;
		}

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr)
				return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
			else
				return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
				func(instance, debugMessenger, pAllocator);
		}

		bool checkValidationLayerSupport()
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			for (const char* layerName : validationLayers)
			{
				bool layerFound = false;
				for (const auto& layerProperties : availableLayers)
				{
					if (strcmp(layerName, layerProperties.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}
				if (!layerFound)
					return false;
			}

			return true;
		}

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
		{
			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = debugCallback;
		}

		std::vector<const char*> getRequiredExtensions()
		{
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
			extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

			if (enableValidationLayers)
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);


			return extensions;
		}

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
		{
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return availablePresentMode;
			}

			return VK_PRESENT_MODE_FIFO_KHR;
		}

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
				return capabilities.currentExtent;
			else
			{
				int width, height;
				glfwGetFramebufferSize(s_CurrentWindow, &width, &height);

				VkExtent2D actualExtent =
				{
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
		{
			for (VkFormat format : candidates)
			{
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties(s_VulkanBackends->physicalDevice, format, &props);

				if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
					return format;
				else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
					return format;
			}

			CHONPS_CORE_ASSERT(false, "Failed to find supported format!"); return VkFormat();
		}

		VkFormat findDepthFormat()
		{
			return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		}

		bool hasStencilComponent(VkFormat format)
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

		void deviceProperties(VkPhysicalDevice device)
		{
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			CHONPS_CORE_INFO("Vulkan Info:");
			CHONPS_CORE_INFO("\tVendor: {0}", deviceProperties.vendorID);
			CHONPS_CORE_INFO("\tRenderer: {0}", deviceProperties.deviceName);
			CHONPS_CORE_INFO("\tVersion: {0}", deviceProperties.driverVersion);
		}
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		CleanupSwapChain();

		if (!m_VulkanBackends->graphicsPipeline.empty())
		{
			for (auto& pipeline : m_VulkanBackends->graphicsPipeline)
			{
				vkDestroyPipeline(m_VulkanBackends->device, pipeline.second.pipeline, nullptr);
				vkDestroyPipelineLayout(m_VulkanBackends->device, pipeline.second.pipelineLayout, nullptr);
			}
		}

		if (m_VulkanBackends->bufferArray != VK_NULL_HANDLE)
			vkDestroyBuffer(m_VulkanBackends->device, m_VulkanBackends->bufferArray, nullptr);
		if (m_VulkanBackends->bufferArrayMemory != VK_NULL_HANDLE)
			vkFreeMemory(m_VulkanBackends->device, m_VulkanBackends->bufferArrayMemory, nullptr);

		if (!m_VulkanBackends->submitUniformBuffers.empty())
		{
			for (auto& buffers : m_VulkanBackends->submitUniformBuffers)
			{
				for (auto& buffer : buffers)
					vkDestroyBuffer(m_VulkanBackends->device, buffer, nullptr);
			}
		}
		if (!m_VulkanBackends->submitUniformBuffersMemory.empty())
		{
			for (auto& bufferMemories : m_VulkanBackends->submitUniformBuffersMemory)
			{
				for (auto& bufferMemory : bufferMemories)
					vkFreeMemory(m_VulkanBackends->device, bufferMemory, nullptr);
			}
		}
		m_VulkanBackends->submitUniformBuffersMapped.clear();

		// Multithreading CommandBuffers
		if (m_VulkanBackends->multithreadingEnabled)
		{
			for (auto& thread : m_ThreadData)
			{
				vkFreeCommandBuffers(m_VulkanBackends->device, thread.commandPool, static_cast<uint32_t>(thread.commandBuffers.size()), thread.commandBuffers.data());
				vkDestroyCommandPool(m_VulkanBackends->device, thread.commandPool, nullptr);
			}
		}

		vkDestroySampler(m_VulkanBackends->device, m_VulkanBackends->nullDataTexture.textureSampler, nullptr);
		vkDestroyImageView(m_VulkanBackends->device, m_VulkanBackends->nullDataTexture.textureImageView, nullptr);
		vkDestroyImage(m_VulkanBackends->device, m_VulkanBackends->nullDataTexture.textureImage, nullptr);
		vkFreeMemory(m_VulkanBackends->device, m_VulkanBackends->nullDataTexture.textureImageMemory, nullptr);

		vkDestroyDescriptorPool(m_VulkanBackends->device, m_VulkanBackends->descriptorPool, nullptr);

		vkDestroyDescriptorSetLayout(m_VulkanBackends->device, m_VulkanBackends->samplerDescriptorSetsLayout, nullptr);

		for (size_t i = 0; i < m_VulkanBackends->maxFramesInFlight; i++)
		{
			vkDestroySemaphore(m_VulkanBackends->device, m_VulkanBackends->renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_VulkanBackends->device, m_VulkanBackends->imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_VulkanBackends->device, m_VulkanBackends->inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(m_VulkanBackends->device, m_VulkanBackends->commandPool, nullptr);

		vkDestroyRenderPass(m_VulkanBackends->device, m_VulkanBackends->renderPass, nullptr);

		vkDestroyDevice(m_VulkanBackends->device, nullptr);

		if (enableValidationLayers)
			vkUtils::DestroyDebugUtilsMessengerEXT(m_VulkanBackends->instance, m_VulkanBackends->debugMessenger, nullptr);

		vkDestroySurfaceKHR(m_VulkanBackends->instance, m_VulkanBackends->surface, nullptr);
		vkDestroyInstance(m_VulkanBackends->instance, nullptr);
	}

	void VulkanRendererAPI::Init()
	{
		m_VulkanBackends = std::make_shared<VulkanBackends>();
		s_VulkanBackends = m_VulkanBackends;
		vkSpec::initStandardVulkanPipelineSpecification();
		vkSpec::fillQueueCountIDs();

		uint32_t threadCount = std::thread::hardware_concurrency();
		m_ThreadPool.setThreadCount(threadCount);
		m_ThreadData.resize(threadCount);
		m_VulkanBackends->numThreads = threadCount;

		CreateInstance();
		SetUpDebugMessenger();
		CreateSurface();
		SetPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateCommandPool();
		CreateDescriptorPool();
		CreateDepthResources();
		CreateFramebuffers();
		CreateCommandBuffers();
		CreateSyncObjects();

		if (m_VulkanBackends->multithreadingEnabled)
		{
			for (uint32_t i = 0; i < m_VulkanBackends->numThreads; i++)
			{
				ThreadData* thread = &m_ThreadData[i];
				thread->commandBuffers.resize(m_VulkanBackends->maxFramesInFlight);

				VkCommandPoolCreateInfo cmdPoolInfo{};
				cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				cmdPoolInfo.queueFamilyIndex = m_VulkanBackends->queueFamiliyIndices.graphicsFamily.value();
				cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				vkCreateCommandPool(m_VulkanBackends->device, &cmdPoolInfo, nullptr, &thread->commandPool);

				VkCommandBufferAllocateInfo secondaryCmdBufAllocateInfo{};
				secondaryCmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				secondaryCmdBufAllocateInfo.commandPool = thread->commandPool;
				secondaryCmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
				secondaryCmdBufAllocateInfo.commandBufferCount = static_cast<uint32_t>(thread->commandBuffers.size());
				vkAllocateCommandBuffers(m_VulkanBackends->device, &secondaryCmdBufAllocateInfo, thread->commandBuffers.data());
			}
		}

		uint32_t textureData = 0x00000000;
		m_VulkanBackends->nullDataTexture = vkSpec::createSingleDataTexture(1, 1, &textureData);

		CmdSetVertexInputEXT = (PFN_vkCmdSetVertexInputEXT)vkGetInstanceProcAddr(m_VulkanBackends->instance, "vkCmdSetVertexInputEXT");
	}

	void VulkanRendererAPI::CreateInstance()
	{
		if (enableValidationLayers)
		{
			CHONPS_CORE_INFO("Vulkan validation layers enabled");
			CHONPS_CORE_ASSERT(vkUtils::checkValidationLayerSupport(), "validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "CHONPS Framework";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
		appInfo.pEngineName = "CHONPS";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = vkUtils::getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();


		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			vkUtils::populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VulkanBackends->instance);
		if (vkCreateInstance(&createInfo, nullptr, &m_VulkanBackends->instance) == VK_SUCCESS)
			CHONPS_CORE_INFO("Vulkan Instance Created");
		else
			CHONPS_CORE_ERROR("ERROR: VULKAN: Failed to Initialize Instance!");
	}

	bool VulkanRendererAPI::SetUpDebugMessenger()
	{
		if (!enableValidationLayers) return false;
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = vkUtils::debugCallback;
		createInfo.pUserData = nullptr;

		CHONPS_CORE_ASSERT(vkUtils::CreateDebugUtilsMessengerEXT(m_VulkanBackends->instance, &createInfo, nullptr, &m_VulkanBackends->debugMessenger) == VK_SUCCESS, "Failed to set up debug messenger!");
		return true;
	}

	void VulkanRendererAPI::CreateSurface()
	{
		CHONPS_CORE_ASSERT(glfwCreateWindowSurface(m_VulkanBackends->instance, s_CurrentWindow, nullptr, &m_VulkanBackends->surface) == VK_SUCCESS, "Failed to Create Window Surface!");
	}

	void VulkanRendererAPI::SetPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VulkanBackends->instance, &deviceCount, nullptr);
		CHONPS_CORE_ASSERT(deviceCount != 0, "Failed to find GPU devices with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VulkanBackends->instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				m_VulkanBackends->physicalDevice = device;
				vkUtils::deviceProperties(device);
				break;
			}
		}

		CHONPS_CORE_ASSERT(m_VulkanBackends->physicalDevice != VK_NULL_HANDLE, "Failed to find suitable GPU!");
	}

	SwapChainSupportDetails VulkanRendererAPI::QuerySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_VulkanBackends->surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_VulkanBackends->surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_VulkanBackends->surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_VulkanBackends->surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_VulkanBackends->surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	bool VulkanRendererAPI::IsDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = FindQueueFamilies(device);

		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.complete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	VkSurfaceFormatKHR VulkanRendererAPI::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		return availableFormats[0];
	}

	void VulkanRendererAPI::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(m_VulkanBackends->physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;


		VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures{};
		indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
		indexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
		indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
		indexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
		indexingFeatures.runtimeDescriptorArray = VK_TRUE;
		indexingFeatures.pNext = nullptr;

		VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT deviceInputDynamicStateFeatures{};
		deviceInputDynamicStateFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT;
		deviceInputDynamicStateFeatures.vertexInputDynamicState = true;
		deviceInputDynamicStateFeatures.pNext = &indexingFeatures;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		createInfo.pNext = &deviceInputDynamicStateFeatures;

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else createInfo.enabledLayerCount = 0;

		CHONPS_CORE_ASSERT(vkCreateDevice(m_VulkanBackends->physicalDevice, &createInfo, nullptr, &m_VulkanBackends->device) == VK_SUCCESS, "failed to create logical device!");

		vkGetDeviceQueue(m_VulkanBackends->device, indices.graphicsFamily.value(), 0, &m_VulkanBackends->graphicsQueue);
		vkGetDeviceQueue(m_VulkanBackends->device, indices.presentFamily.value(), 0, &m_VulkanBackends->presentQueue);
	}

	void VulkanRendererAPI::CreateSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_VulkanBackends->physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = vkUtils::chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = vkUtils::chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_VulkanBackends->surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = FindQueueFamilies(m_VulkanBackends->physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		CHONPS_CORE_ASSERT(vkCreateSwapchainKHR(m_VulkanBackends->device, &createInfo, nullptr, &m_VulkanBackends->swapChain) == VK_SUCCESS, "Failed to create swap chain!");

		vkGetSwapchainImagesKHR(m_VulkanBackends->device, m_VulkanBackends->swapChain, &imageCount, nullptr);
		m_VulkanBackends->swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_VulkanBackends->device, m_VulkanBackends->swapChain, &imageCount, m_VulkanBackends->swapChainImages.data());

		m_VulkanBackends->swapChainImageFormat = surfaceFormat.format;
		m_VulkanBackends->swapChainExtent = extent;
	}

	void VulkanRendererAPI::CreateImageViews()
	{
		m_VulkanBackends->swapChainImageViews.resize(m_VulkanBackends->swapChainImages.size());

		for (size_t i = 0; i < m_VulkanBackends->swapChainImages.size(); i++)
			m_VulkanBackends->swapChainImageViews[i] = vkSpec::createImageView(m_VulkanBackends->swapChainImages[i], m_VulkanBackends->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void VulkanRendererAPI::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_VulkanBackends->swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = vkUtils::findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		CHONPS_CORE_ASSERT(vkCreateRenderPass(m_VulkanBackends->device, &renderPassInfo, nullptr, &m_VulkanBackends->renderPass) == VK_SUCCESS, "Failed to create render pass!");
	}

	void VulkanRendererAPI::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = m_VulkanBackends->maxDescriptorCounts;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = m_VulkanBackends->maxDescriptorCounts;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = m_VulkanBackends->maxDescriptorCounts;

		CHONPS_CORE_ASSERT(vkCreateDescriptorPool(m_VulkanBackends->device, &poolInfo, nullptr, &m_VulkanBackends->descriptorPool) == VK_SUCCESS, "Failed to create descriptor pool!");

		VkDescriptorPoolSize pool_sizes_bindless[] =
		{
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_VulkanBackends->maxFramesInFlight }
		};
	}

	void VulkanRendererAPI::CreateFramebuffers()
	{
		m_VulkanBackends->swapChainFramebuffers.resize(m_VulkanBackends->swapChainImageViews.size());

		for (size_t i = 0; i < m_VulkanBackends->swapChainImageViews.size(); i++)
		{
			std::array<VkImageView, 2> attachments =
			{
				m_VulkanBackends->swapChainImageViews[i],
				m_VulkanBackends->depthImageView
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_VulkanBackends->renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_VulkanBackends->swapChainExtent.width;
			framebufferInfo.height = m_VulkanBackends->swapChainExtent.height;
			framebufferInfo.layers = 1;

			CHONPS_CORE_ASSERT(vkCreateFramebuffer(m_VulkanBackends->device, &framebufferInfo, nullptr, &m_VulkanBackends->swapChainFramebuffers[i]) == VK_SUCCESS, "Failed to create framebuffer!");
		}
	}

	void VulkanRendererAPI::CreateCommandPool()
	{
		m_VulkanBackends->queueFamiliyIndices = FindQueueFamilies(m_VulkanBackends->physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = m_VulkanBackends->queueFamiliyIndices.graphicsFamily.value();

		CHONPS_CORE_ASSERT(vkCreateCommandPool(m_VulkanBackends->device, &poolInfo, nullptr, &m_VulkanBackends->commandPool) == VK_SUCCESS, "Failed to create command pool!");
	}

	void VulkanRendererAPI::CreateDepthResources()
	{
		VkFormat depthFormat = vkUtils::findDepthFormat();

		vkSpec::createImage(s_VulkanBackends->swapChainExtent.width, s_VulkanBackends->swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VulkanBackends->depthImage, m_VulkanBackends->depthImageMemory);
		m_VulkanBackends->depthImageView = vkSpec::createImageView(m_VulkanBackends->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		vkSpec::transitionImageLayout(m_VulkanBackends->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	void VulkanRendererAPI::CreateCommandBuffers()
	{
		m_VulkanBackends->commandBuffers.resize(m_VulkanBackends->maxFramesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_VulkanBackends->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)m_VulkanBackends->commandBuffers.size();

		CHONPS_CORE_ASSERT(vkAllocateCommandBuffers(m_VulkanBackends->device, &allocInfo, m_VulkanBackends->commandBuffers.data()) == VK_SUCCESS, "Failed to allocate command buffers!");
	}

	void VulkanRendererAPI::ThreadRender(uint32_t threadIndex, uint32_t cmdBufferIndex, VkCommandBufferInheritanceInfo inheritanceInfo)
	{
		ThreadData* thread = &m_ThreadData[threadIndex];

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		beginInfo.pInheritanceInfo = &inheritanceInfo;

		VkCommandBuffer cmdBuffer = thread->commandBuffers[cmdBufferIndex];

		vkBeginCommandBuffer(cmdBuffer, &beginInfo);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_VulkanBackends->swapChainExtent.width);
		viewport.height = static_cast<float>(m_VulkanBackends->swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_VulkanBackends->swapChainExtent;
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

		CmdSetVertexInputEXT(cmdBuffer, 1, &m_VulkanBackends->bindingDescriptions, static_cast<uint32_t>(m_VulkanBackends->attributeDescriptions.size()), m_VulkanBackends->attributeDescriptions.data());

		VertexArray* lastVertexArray = nullptr;
		uint32_t lastShaderIndex = 0; bool firstTimeBindShader = true;

		for (auto& drawCommandData : thread->drawCommandData)
		{
			DrawCommand drawCmd = drawCommandData.drawCmd;

			if (drawCmd.shaderIndex != lastShaderIndex || firstTimeBindShader == true)
			{
				vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanBackends->graphicsPipeline[drawCmd.shaderIndex].pipeline);
				firstTimeBindShader = false;
			}
			lastShaderIndex = drawCmd.shaderIndex;

			if (drawCmd.vertexArray != lastVertexArray)
			{
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &drawCommandData.vertexBuffer, offsets);
				vkCmdBindIndexBuffer(cmdBuffer, drawCommandData.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			}
			lastVertexArray = drawCmd.vertexArray;

			VkDescriptorSet descriptorSets[] =
			{
				m_VulkanBackends->submitDescriptorSets[drawCmd.uniformBufferIndex][m_VulkanBackends->currentFrame],
				drawCmd.textureIDs.empty() ? m_VulkanBackends->nullSamplerDescriptorSets[m_VulkanBackends->currentFrame] : m_VulkanBackends->samplerDescriptorSets[drawCmd.textureArrayIndex][m_VulkanBackends->currentFrame]
			};

			vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanBackends->graphicsPipeline[drawCmd.shaderIndex].pipelineLayout, 0, 2, descriptorSets, 0, nullptr);
			vkCmdPushConstants(cmdBuffer, m_VulkanBackends->graphicsPipeline[drawCmd.shaderIndex].pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantTextureIndexData), &m_VulkanBackends->textureIndexConstant);

			uint32_t indexCount = drawCmd.vertexArray->GetIndexCount();
			vkCmdDrawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0); // main draw call
		}

		vkEndCommandBuffer(cmdBuffer);
	}

	void VulkanRendererAPI::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const uint32_t& count)
	{
		std::vector<VkCommandBuffer> secondaryCommandBuffers;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		CHONPS_CORE_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_VulkanBackends->renderPass;
		renderPassInfo.framebuffer = m_VulkanBackends->swapChainFramebuffers[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_VulkanBackends->swapChainExtent;

		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &m_VulkanBackends->clearColor;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = m_VulkanBackends->clearColor.color;
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, (m_VulkanBackends->multithreadingEnabled && m_VulkanBackends->drawCallCount >= m_VulkanBackends->numThreads) ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : VK_SUBPASS_CONTENTS_INLINE);

		if (m_VulkanBackends->multithreadingEnabled && m_VulkanBackends->drawCallCount >= m_VulkanBackends->numThreads)
		{
			// Secondary CommandBuffers
			VkCommandBufferInheritanceInfo inheritanceInfo{};
			inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			inheritanceInfo.renderPass = m_VulkanBackends->renderPass;
			inheritanceInfo.framebuffer = m_VulkanBackends->swapChainFramebuffers[imageIndex];

			VkCommandBufferBeginInfo inheritanceBeginInfo{};
			inheritanceBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			inheritanceBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			inheritanceBeginInfo.pInheritanceInfo = &inheritanceInfo;

			for (uint32_t t = 0; t < m_VulkanBackends->numThreads; t++)
			{
				m_ThreadPool.threads[t]->addJob([=] { ThreadRender(t, m_VulkanBackends->currentFrame, inheritanceInfo); });
			}

			m_ThreadPool.wait();

			for (uint32_t t = 0; t < m_VulkanBackends->numThreads; t++)
			{
				secondaryCommandBuffers.push_back(m_ThreadData[t].commandBuffers[m_VulkanBackends->currentFrame]);
			}

			// Execute render commands from the secondary command buffer
			vkCmdExecuteCommands(commandBuffer, static_cast<uint32_t>(secondaryCommandBuffers.size()), secondaryCommandBuffers.data());
		}
		else // Multithreading disabled
		{
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(m_VulkanBackends->swapChainExtent.width);
			viewport.height = static_cast<float>(m_VulkanBackends->swapChainExtent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = m_VulkanBackends->swapChainExtent;
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			CmdSetVertexInputEXT(commandBuffer, 1, &m_VulkanBackends->bindingDescriptions, static_cast<uint32_t>(m_VulkanBackends->attributeDescriptions.size()), m_VulkanBackends->attributeDescriptions.data());

			VertexArray* lastVertexArray = nullptr;
			uint32_t lastShaderIndex = 0; bool firstTimeBindShader = true;

			for (auto& drawCommand : m_DrawList.drawCommands)
			{
				if (drawCommand.shaderIndex != lastShaderIndex || firstTimeBindShader == true)
				{
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanBackends->graphicsPipeline[drawCommand.shaderIndex].pipeline);
					firstTimeBindShader = false;
				}
				lastShaderIndex = drawCommand.shaderIndex;

				if (drawCommand.vertexArray != lastVertexArray)
				{
					drawCommand.vertexArray->Bind();
					VkDeviceSize offsets[] = { 0 };
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VulkanBackends->vertexBuffer, offsets);
					vkCmdBindIndexBuffer(commandBuffer, m_VulkanBackends->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
				}
				lastVertexArray = drawCommand.vertexArray;

				VkDescriptorSet descriptorSets[] =
				{
					s_VulkanBackends->submitDescriptorSets[drawCommand.uniformBufferIndex][s_VulkanBackends->currentFrame],
					drawCommand.textureIDs.empty() ? s_VulkanBackends->nullSamplerDescriptorSets[s_VulkanBackends->currentFrame] : s_VulkanBackends->samplerDescriptorSets[drawCommand.textureArrayIndex][s_VulkanBackends->currentFrame]
				};

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_VulkanBackends->graphicsPipeline[drawCommand.shaderIndex].pipelineLayout, 0, 2, descriptorSets, 0, nullptr);
				vkCmdPushConstants(commandBuffer, s_VulkanBackends->graphicsPipeline[drawCommand.shaderIndex].pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantTextureIndexData), &s_VulkanBackends->textureIndexConstant);

				uint32_t indexCount = drawCommand.vertexArray->GetIndexCount();
				vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0); // main draw call
			}
		}

		vkCmdEndRenderPass(commandBuffer);

		CHONPS_CORE_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed to record command buffer!");
	}

	void VulkanRendererAPI::CreateSyncObjects()
	{
		m_VulkanBackends->imageAvailableSemaphores.resize(m_VulkanBackends->maxFramesInFlight);
		m_VulkanBackends->renderFinishedSemaphores.resize(m_VulkanBackends->maxFramesInFlight);
		m_VulkanBackends->inFlightFences.resize(m_VulkanBackends->maxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < m_VulkanBackends->maxFramesInFlight; i++)
		{
			CHONPS_CORE_ASSERT(vkCreateSemaphore(m_VulkanBackends->device, &semaphoreInfo, nullptr, &m_VulkanBackends->imageAvailableSemaphores[i]) == VK_SUCCESS &&
				vkCreateSemaphore(m_VulkanBackends->device, &semaphoreInfo, nullptr, &m_VulkanBackends->renderFinishedSemaphores[i]) == VK_SUCCESS &&
				vkCreateFence(m_VulkanBackends->device, &fenceInfo, nullptr, &m_VulkanBackends->inFlightFences[i]) == VK_SUCCESS, "Failed to create semaphores!");
		}
	}

	bool VulkanRendererAPI::CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}

	QueueFamilyIndices VulkanRendererAPI::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int familyCount = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = familyCount;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, familyCount, m_VulkanBackends->surface, &presentSupport);

			if (presentSupport)
				indices.presentFamily = familyCount;

			if (indices.complete())
				break;

			familyCount++;
		}

		return indices;
	}

	void VulkanRendererAPI::RecreateSwapChain()
	{
		vkDeviceWaitIdle(m_VulkanBackends->device);

		CleanupSwapChain();
		CreateSwapChain();
		CreateImageViews();
		CreateDepthResources();
		CreateFramebuffers();
	}

	void VulkanRendererAPI::CleanupSwapChain()
	{
		vkDestroyImageView(m_VulkanBackends->device, m_VulkanBackends->depthImageView, nullptr);
		vkDestroyImage(m_VulkanBackends->device, m_VulkanBackends->depthImage, nullptr);
		vkFreeMemory(m_VulkanBackends->device, m_VulkanBackends->depthImageMemory, nullptr);

		for (size_t i = 0; i < m_VulkanBackends->swapChainFramebuffers.size(); i++)
			vkDestroyFramebuffer(m_VulkanBackends->device, m_VulkanBackends->swapChainFramebuffers[i], nullptr);

		for (size_t i = 0; i < m_VulkanBackends->swapChainImageViews.size(); i++)
			vkDestroyImageView(m_VulkanBackends->device, m_VulkanBackends->swapChainImageViews[i], nullptr);

		vkDestroySwapchainKHR(m_VulkanBackends->device, m_VulkanBackends->swapChain, nullptr);
	}

	void VulkanRendererAPI::CreateOrUpdateUniformBuffer(uint32_t currentImage, DrawCommand drawCommand)
	{
		if (m_VulkanBackends->uboData == nullptr || m_VulkanBackends->uboSize == 0)
			return;

		memcpy(m_VulkanBackends->uniformBuffersMapped[currentImage], static_cast<const char*>(m_VulkanBackends->uboData) + m_VulkanBackends->uboOffset, m_VulkanBackends->uboSize - m_VulkanBackends->uboOffset);

		if (m_VulkanBackends->drawCallCount > m_VulkanBackends->drawCallCountTotal || m_VulkanBackends->firstTimeDrawCallCount)
		{
			// Allocate Descriptor Sets
			std::vector<VkDescriptorSetLayout> descriptorSetLayouts(m_VulkanBackends->maxFramesInFlight, m_VulkanBackends->descriptorSetLayout);
			VkDescriptorSetAllocateInfo descriptorAllocInfo{};
			descriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorAllocInfo.descriptorPool = m_VulkanBackends->descriptorPool;
			descriptorAllocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			descriptorAllocInfo.pSetLayouts = descriptorSetLayouts.data();

			std::vector<VkDescriptorSet> desciptorSets; desciptorSets.resize(m_VulkanBackends->maxFramesInFlight);
			CHONPS_CORE_ASSERT(vkAllocateDescriptorSets(m_VulkanBackends->device, &descriptorAllocInfo, desciptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");
			m_VulkanBackends->submitDescriptorSets.push_back(desciptorSets);

			// Create Uniform Buffers
			std::vector<VkBuffer> uniformBuffers; uniformBuffers.resize(m_VulkanBackends->maxFramesInFlight);
			std::vector<VkDeviceMemory> uniformBuffersMemory; uniformBuffersMemory.resize(m_VulkanBackends->maxFramesInFlight);
			std::vector<void*> uniformBuffersMapped; uniformBuffersMapped.resize(m_VulkanBackends->maxFramesInFlight);
			for (size_t i = 0; i < m_VulkanBackends->maxFramesInFlight; i++)
			{
				vkSpec::createBuffer(m_VulkanBackends->uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
				vkMapMemory(m_VulkanBackends->device, uniformBuffersMemory[i], 0, m_VulkanBackends->uboSize, 0, &uniformBuffersMapped[i]);
			}
			m_VulkanBackends->submitUniformBuffers.push_back(uniformBuffers);
			m_VulkanBackends->submitUniformBuffersMemory.push_back(uniformBuffersMemory);
			m_VulkanBackends->submitUniformBuffersMapped.push_back(uniformBuffersMapped);
		}

		memcpy(m_VulkanBackends->submitUniformBuffersMapped[drawCommand.uniformBufferIndex][currentImage], m_VulkanBackends->uboData, m_VulkanBackends->uboSize);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_VulkanBackends->submitUniformBuffers[drawCommand.uniformBufferIndex][currentImage];
		bufferInfo.offset = 0;
		bufferInfo.range = m_VulkanBackends->uboSize;

		VkWriteDescriptorSet descriptorWrites{};
		descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites.dstSet = m_VulkanBackends->submitDescriptorSets[drawCommand.uniformBufferIndex][currentImage];
		descriptorWrites.dstBinding = m_VulkanBackends->currentDescriptorSetBinding;
		descriptorWrites.dstArrayElement = 0;
		descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites.descriptorCount = 1;
		descriptorWrites.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(m_VulkanBackends->device, 1, &descriptorWrites, 0, nullptr);
	}

	void VulkanRendererAPI::CreateOrUpdateTextureImage(uint32_t currentImage)
	{

		for (auto& drawCommand : m_DrawList.drawCommands)
		{
			if (!drawCommand.textureIDs.empty() && (m_VulkanBackends->drawCallCount > m_VulkanBackends->drawCallCountTotal || m_VulkanBackends->firstTimeDrawCallCount))
			{
				std::vector<VkDescriptorSetLayout> samplerLayouts(m_VulkanBackends->maxFramesInFlight, m_VulkanBackends->samplerDescriptorSetsLayout);
				VkDescriptorSetAllocateInfo samplerAllocInfo{};
				samplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				samplerAllocInfo.descriptorPool = m_VulkanBackends->descriptorPool;
				samplerAllocInfo.descriptorSetCount = static_cast<uint32_t>(samplerLayouts.size());
				samplerAllocInfo.pSetLayouts = samplerLayouts.data();

				std::vector<VkDescriptorSet> descriptorSets{}; descriptorSets.resize(m_VulkanBackends->maxFramesInFlight);
				CHONPS_CORE_ASSERT(vkAllocateDescriptorSets(m_VulkanBackends->device, &samplerAllocInfo, descriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");
				m_VulkanBackends->samplerDescriptorSets.push_back(descriptorSets);
			}
			else if (drawCommand.textureIDs.empty()) return;

			for (auto& texBind : drawCommand.textureIDs) // Update Descriptor Sets
			{
				VulkanTextureData texData = m_VulkanBackends->textureImages[texBind];

				m_VulkanBackends->imageInfos[texData.unit].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				m_VulkanBackends->imageInfos[texData.unit].imageView = texData.textureImageView;
				m_VulkanBackends->imageInfos[texData.unit].sampler = texData.textureSampler;

				std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = m_VulkanBackends->samplerDescriptorSets[drawCommand.textureArrayIndex][currentImage];
				descriptorWrites[0].dstBinding = m_VulkanBackends->textureBinding;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				descriptorWrites[0].descriptorCount = m_VulkanBackends->maxTextures;
				descriptorWrites[0].pImageInfo = m_VulkanBackends->imageInfos.data();

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = m_VulkanBackends->samplerDescriptorSets[drawCommand.textureArrayIndex][currentImage];
				descriptorWrites[1].dstBinding = m_VulkanBackends->samplerBinding;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				descriptorWrites[1].descriptorCount = m_VulkanBackends->maxTextures;
				descriptorWrites[1].pImageInfo = m_VulkanBackends->imageInfos.data();

				vkUpdateDescriptorSets(m_VulkanBackends->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}
		}
	}

	void VulkanRendererAPI::ClearColor(const float r, const float g, const float b, const float w)
	{
		m_VulkanBackends->clearColor = { {{r, g, b, w}} };
	}

	void VulkanRendererAPI::Clear()
	{

	}

	void VulkanRendererAPI::Draw(const uint32_t& count)
	{
		m_VulkanBackends->indexCount += count;
	}

	void VulkanRendererAPI::Draw(VertexArray* vertexArray)
	{
		int width, height;
		glfwGetFramebufferSize(s_CurrentWindow, &width, &height);
		if (width == 0 || height == 0)
			return;

		m_VulkanBackends->drawCallCount++;

		DrawCommand drawCommand{};
		drawCommand.vertexArray = vertexArray;
		drawCommand.textureIDs = std::vector<uint32_t>(m_VulkanBackends->currentBindedTextureImages.begin(), m_VulkanBackends->currentBindedTextureImages.end());

		drawCommand.shaderIndex = m_VulkanBackends->currentBindedGraphicsPipeline;
		drawCommand.textureArrayIndex = m_VulkanBackends->drawCallCount - 1;
		drawCommand.uniformBufferIndex = m_VulkanBackends->drawCallCount - 1;
		m_DrawList.drawCommands.emplace_back(drawCommand);

		CreateOrUpdateUniformBuffer(m_VulkanBackends->currentFrame, drawCommand);

		if (m_VulkanBackends->multithreadingEnabled)
		{
			drawCommand.vertexArray->Bind();
			ThreadDrawCmdData threadDrawCmdData{};
			threadDrawCmdData.drawCmd = drawCommand;
			threadDrawCmdData.vertexBuffer = m_VulkanBackends->vertexBuffer;
			threadDrawCmdData.indexBuffer = m_VulkanBackends->indexBuffer;

			m_ThreadData[m_VulkanBackends->threadIndex].drawCommandData.emplace_back(threadDrawCmdData); // Place draw cmd data in thread
			m_VulkanBackends->threadIndex = (m_VulkanBackends->threadIndex + 1) % m_VulkanBackends->numThreads; // Cycle to the next threads
		}

		m_VulkanBackends->allowClearBindedTextures = true;
	}

	void VulkanRendererAPI::DrawLine(VertexArray* vertexArray)
	{
		int width, height;
		glfwGetFramebufferSize(s_CurrentWindow, &width, &height);
		if (width == 0 || height == 0)
			return;

		DrawCommand drawCommand{};
		drawCommand.vertexArray = vertexArray;

		drawCommand.shaderIndex = m_VulkanBackends->currentBindedGraphicsPipeline;
		drawCommand.uniformBufferIndex = m_VulkanBackends->drawCallCount - 1;
		m_DrawList.drawCommands.emplace_back(drawCommand);
	}

	void VulkanRendererAPI::BeginNextFrame()
	{
		int width, height;
		glfwGetFramebufferSize(s_CurrentWindow, &width, &height);
		if (width == 0 || height == 0)
			return;

		// Clean up rendering data for next frame
		m_DrawList.clear();
		m_VulkanBackends->indexCount = 0;
		m_VulkanBackends->drawCallCount = 0;
		m_VulkanBackends->indexCount = 0;
		m_VulkanBackends->currentBindedTextureImages.clear();

		for (auto& thread : m_ThreadData)
			thread.drawCommandData.clear();

		vkWaitForFences(m_VulkanBackends->device, 1, &m_VulkanBackends->inFlightFences[m_VulkanBackends->currentFrame], VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(m_VulkanBackends->device, m_VulkanBackends->swapChain, UINT64_MAX, m_VulkanBackends->imageAvailableSemaphores[m_VulkanBackends->currentFrame], VK_NULL_HANDLE, &m_VulkanBackends->imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		CHONPS_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image!");
	}

	void VulkanRendererAPI::DrawSubmit()
	{
		int width, height;
		glfwGetFramebufferSize(s_CurrentWindow, &width, &height);
		if (width == 0 || height == 0)
			return;

		CHONPS_CORE_ASSERT(m_VulkanBackends->uboData != nullptr, "ERROR: UNIFORM_BUFFER: uniform data was nullptr!");

		CreateOrUpdateTextureImage(m_VulkanBackends->currentFrame);

		m_VulkanBackends->firstTimeDrawCallCount = false; // After the first render submit call and counting the total draw calls
		m_VulkanBackends->allowClearBindedTextures = true; // Allow all texture binds to be cleared in currentlyBindedTextureImages

		m_VulkanBackends->drawCallCountTotal = m_VulkanBackends->drawCallCount;

		// Only reset the fence if we are submitting work
		vkResetFences(m_VulkanBackends->device, 1, &m_VulkanBackends->inFlightFences[m_VulkanBackends->currentFrame]);

		m_VulkanBackends->bufferArrayIndexOffset = m_DrawList.vertexSize;
		m_VulkanBackends->indexCount = static_cast<uint32_t>(m_DrawList.indexCount);

		vkResetCommandBuffer(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
		RecordCommandBuffer(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], m_VulkanBackends->imageIndex, m_VulkanBackends->indexCount);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_VulkanBackends->imageAvailableSemaphores[m_VulkanBackends->currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame];

		VkSemaphore signalSemaphores[] = { m_VulkanBackends->renderFinishedSemaphores[m_VulkanBackends->currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		CHONPS_CORE_ASSERT(vkQueueSubmit(m_VulkanBackends->graphicsQueue, 1, &submitInfo, m_VulkanBackends->inFlightFences[m_VulkanBackends->currentFrame]) == VK_SUCCESS, "Failed to submit draw command buffer!");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_VulkanBackends->swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_VulkanBackends->imageIndex;
		presentInfo.pResults = nullptr; // Optional

		VkResult result = vkQueuePresentKHR(m_VulkanBackends->presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_VulkanBackends->framebufferResized)
		{
			m_VulkanBackends->framebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
			CHONPS_CORE_ASSERT(false, "Failed to present swap chain image!");

		m_VulkanBackends->currentFrame = (m_VulkanBackends->currentFrame + 1) % m_VulkanBackends->maxFramesInFlight; // Modulo (%) will loop the frame index back to beggining after every Max Frame
	}

	void VulkanRendererAPI::FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height)
	{

	}

	void VulkanRendererAPI::EnableCullFace()
	{

	}

	void VulkanRendererAPI::DisableCullFace()
	{

	}

	namespace vkSpec
	{
		VkVertexInputBindingDescription2EXT getBindingDescription(uint32_t stride)
		{
			VkVertexInputBindingDescription2EXT bindingDescription{};
			bindingDescription.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
			bindingDescription.binding = 0;
			bindingDescription.divisor = 1;
			bindingDescription.stride = stride;
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		VkVertexInputAttributeDescription2EXT getAttributeDescriptions(uint32_t layout, VkFormat type, uint32_t stride, void* offset)
		{
			VkVertexInputAttributeDescription2EXT attributeDescriptions{};
			attributeDescriptions.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
			attributeDescriptions.binding = 0;
			attributeDescriptions.location = layout;
			attributeDescriptions.format = type;
			attributeDescriptions.offset = static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(offset));
			s_VulkanBackends->bindingDescriptions = getBindingDescription(stride);

			return attributeDescriptions;
		}

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(s_VulkanBackends->physicalDevice, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return i;
			}

			CHONPS_CORE_ASSERT(false, "Failed to find suitable memory type!");
			return -1;
		}

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VkDevice device = s_VulkanBackends->device;

			CHONPS_CORE_ASSERT(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) == VK_SUCCESS, "Failed to create buffer!");

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

			CHONPS_CORE_ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) == VK_SUCCESS, "Failed to allocate buffer memory!");

			vkBindBufferMemory(device, buffer, bufferMemory, 0);
		}

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			endSingleTimeCommands(commandBuffer);
		}

		VkCommandBuffer beginSingleTimeCommands()
		{
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = s_VulkanBackends->commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(s_VulkanBackends->device, &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			return commandBuffer;
		}

		void endSingleTimeCommands(VkCommandBuffer commandBuffer)
		{
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(s_VulkanBackends->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(s_VulkanBackends->graphicsQueue);

			vkFreeCommandBuffers(s_VulkanBackends->device, s_VulkanBackends->commandPool, 1, &commandBuffer);
		}

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
		{
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (vkUtils::hasStencilComponent(format))
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			else
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}
			else CHONPS_CORE_ASSERT(false, "Unsupported layout transition!");

			vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			endSingleTimeCommands(commandBuffer);
		}

		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
		{
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkBufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = { width, height, 1 };

			vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			endSingleTimeCommands(commandBuffer);
		}

		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = format;
			viewInfo.subresourceRange.aspectMask = aspectFlags;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VkImageView imageView = VK_NULL_HANDLE;
			CHONPS_CORE_ASSERT(vkCreateImageView(s_VulkanBackends->device, &viewInfo, nullptr, &imageView) == VK_SUCCESS, "Failed to create texture image view!");

			return imageView;
		}

		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format;
			imageInfo.tiling = tiling;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = usage;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateImage(s_VulkanBackends->device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(s_VulkanBackends->device, image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(s_VulkanBackends->device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate image memory!");
			}

			vkBindImageMemory(s_VulkanBackends->device, image, imageMemory, 0);
		}

		VulkanTextureData createSingleDataTexture(uint32_t width, uint32_t height, void* data)
		{
			VulkanTextureData vulkanTexture;

			VulkanBackends* m_VulkanBackends = getVulkanBackends();

			VkFormat format = renderGetGammaCorrection() == true ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;

			VkDeviceSize imageSize = width * height * 4;

			vkSpec::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* bufferData;
			vkMapMemory(m_VulkanBackends->device, stagingBufferMemory, 0, imageSize, 0, &bufferData);
			memcpy(bufferData, data, static_cast<size_t>(imageSize));
			vkUnmapMemory(m_VulkanBackends->device, stagingBufferMemory);

			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = static_cast<uint32_t>(width);
			imageInfo.extent.height = static_cast<uint32_t>(height);
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.flags = 0; // Optional

			CHONPS_CORE_ASSERT(vkCreateImage(m_VulkanBackends->device, &imageInfo, nullptr, &vulkanTexture.textureImage) == VK_SUCCESS, "Failed to create image!");

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(m_VulkanBackends->device, vulkanTexture.textureImage, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = vkSpec::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			CHONPS_CORE_ASSERT(vkAllocateMemory(m_VulkanBackends->device, &allocInfo, nullptr, &vulkanTexture.textureImageMemory) == VK_SUCCESS, "Failed to allocate image memory!");

			vkBindImageMemory(m_VulkanBackends->device, vulkanTexture.textureImage, vulkanTexture.textureImageMemory, 0);

			vkSpec::transitionImageLayout(vulkanTexture.textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			vkSpec::copyBufferToImage(stagingBuffer, vulkanTexture.textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

			vkSpec::transitionImageLayout(vulkanTexture.textureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			vkDestroyBuffer(m_VulkanBackends->device, stagingBuffer, nullptr);
			vkFreeMemory(m_VulkanBackends->device, stagingBufferMemory, nullptr);

			vulkanTexture.textureImageView = vkSpec::createImageView(vulkanTexture.textureImage, format, VK_IMAGE_ASPECT_COLOR_BIT);

			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_NEAREST;
			samplerInfo.minFilter = VK_FILTER_NEAREST;

			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(m_VulkanBackends->physicalDevice, &properties);
			samplerInfo.anisotropyEnable = VK_TRUE;
			samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;

			CHONPS_CORE_ASSERT(vkCreateSampler(m_VulkanBackends->device, &samplerInfo, nullptr, &vulkanTexture.textureSampler) == VK_SUCCESS, "Failed to create texture sampler!");

			return vulkanTexture;
		}

		void fillQueueCountIDs()
		{
			for (uint32_t i = 0; i < 0xffff; i++)
			{
				s_VulkanBackends->shaderCountIDs.push(i);
				s_VulkanBackends->textureCountIDs.push(i);
			}
		}

		void vkImplPrepareDraw()
		{
			VkDescriptorSetLayoutBinding textureLayoutBinding{};
			textureLayoutBinding.binding = s_VulkanBackends->textureBinding;
			textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			textureLayoutBinding.descriptorCount = s_VulkanBackends->maxTextures;
			textureLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
			textureLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutBinding samplerLayoutBinding{};
			samplerLayoutBinding.binding = s_VulkanBackends->samplerBinding;
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			samplerLayoutBinding.descriptorCount = s_VulkanBackends->maxTextures;
			samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
			samplerLayoutBinding.pImmutableSamplers = nullptr;

			std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { textureLayoutBinding, samplerLayoutBinding };
			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
			layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
			layoutInfo.pBindings = layoutBindings.data();

			CHONPS_CORE_ASSERT(vkCreateDescriptorSetLayout(s_VulkanBackends->device, &layoutInfo, nullptr, &s_VulkanBackends->samplerDescriptorSetsLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");


			std::vector<VkDescriptorSetLayout> samplerLayouts(s_VulkanBackends->maxFramesInFlight, s_VulkanBackends->samplerDescriptorSetsLayout);
			VkDescriptorSetAllocateInfo samplerAllocInfo{};
			samplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			samplerAllocInfo.descriptorPool = s_VulkanBackends->descriptorPool;
			samplerAllocInfo.descriptorSetCount = static_cast<uint32_t>(samplerLayouts.size());
			samplerAllocInfo.pSetLayouts = samplerLayouts.data();

			s_VulkanBackends->nullSamplerDescriptorSets.resize(s_VulkanBackends->maxFramesInFlight);
			CHONPS_CORE_ASSERT(vkAllocateDescriptorSets(s_VulkanBackends->device, &samplerAllocInfo, s_VulkanBackends->nullSamplerDescriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");

			std::vector<VkDescriptorImageInfo> imageInfo{};
			imageInfo.resize(s_VulkanBackends->maxTextures);
			for (uint32_t i = 0; i < imageInfo.size(); i++)
			{
				imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo[i].imageView = s_VulkanBackends->nullDataTexture.textureImageView;
				imageInfo[i].sampler = s_VulkanBackends->nullDataTexture.textureSampler;
			}
			s_VulkanBackends->imageInfos = imageInfo;

			for (size_t i = 0; i < s_VulkanBackends->nullSamplerDescriptorSets.size(); i++)
			{
				std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = s_VulkanBackends->nullSamplerDescriptorSets[i];
				descriptorWrites[0].dstBinding = s_VulkanBackends->textureBinding;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				descriptorWrites[0].descriptorCount = s_VulkanBackends->maxTextures;
				descriptorWrites[0].pImageInfo = imageInfo.data();

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = s_VulkanBackends->nullSamplerDescriptorSets[i];
				descriptorWrites[1].dstBinding = s_VulkanBackends->samplerBinding;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				descriptorWrites[1].descriptorCount = s_VulkanBackends->maxTextures;
				descriptorWrites[1].pImageInfo = imageInfo.data();

				vkUpdateDescriptorSets(s_VulkanBackends->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}

			VkPushConstantRange pushConstant{};
			pushConstant.size = sizeof(PushConstantTextureIndexData);
			pushConstant.offset = 0;
			pushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			s_VulkanBackends->pushConstantRange = pushConstant;
			s_VulkanBackends->pushConstantRangeCount++;

			if (!s_VulkanBackends->uboDescriptorSetLayouts.empty())
				for (auto& uboDescriptorSetLayout : s_VulkanBackends->uboDescriptorSetLayouts)
					s_VulkanBackends->toPipelineDescriptorSetLayouts.push_back(uboDescriptorSetLayout.second);
			else CHONPS_CORE_WARN("WARNING: VULKAN: RENDER_PREP: No Uniform Buffer Objects filled out!");

			s_VulkanBackends->toPipelineDescriptorSetLayouts.push_back(s_VulkanBackends->samplerDescriptorSetsLayout);

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 0;
			vertexInputInfo.vertexAttributeDescriptionCount = 0;
			vertexInputInfo.pVertexBindingDescriptions = nullptr;
			vertexInputInfo.pVertexAttributeDescriptions = nullptr;

			auto bindingDescription = vkSpec::getBindingDescription(sizeof(vertex));
			getVulkanBackends()->bindingDescriptions = bindingDescription;

			vkSpec::PipelineSpecification pipeline = vkSpec::getStandardVulkanPipelineSpecification();

			if (!s_VulkanBackends->pipelineShaderStages.empty())
				for (auto& shaderStage : s_VulkanBackends->pipelineShaderStages)
				{
					VkPipelineShaderStageCreateInfo pipelineShaderStageInfo[] = { shaderStage.second.vertexShaderStage, shaderStage.second.fragementShaderStage };
					s_VulkanBackends->graphicsPipeline[shaderStage.first] = createVulkanPipeline(pipeline, pipelineShaderStageInfo, vertexInputInfo);
				}
			else CHONPS_CORE_WARN("WARNING: VULKAN: RENDER_PREP: No shaders or pipelines filled out!");
		}
	}
}