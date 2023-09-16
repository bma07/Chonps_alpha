#include "cepch.h"
#include "VulkanRendererAPI.h"

#include "Core/File.h"
#include "VulkanPipeline.h"
#include "VulkanVertexBuffer.h"

#include <optional>
#include <future>
#include <thread>

const std::vector<const char*> validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_MAINTENANCE1_EXTENSION_NAME,
	VK_KHR_MAINTENANCE3_EXTENSION_NAME,
	VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
	VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
};


namespace Chonps
{
	static GLFWwindow* s_CurrentWindow;
	static std::shared_ptr<VulkanBackends> s_VulkanBackends;
	static VmaAllocator s_VmaAllocator;
	static TextureDescriptorSetBindings s_TextureDescriptorBindings;

	void setCurrentWindowForVulkanWindowSurface(GLFWwindow* window)
	{
		s_CurrentWindow = window;
	}

	VulkanBackends* getVulkanBackends()
	{
		return &(*s_VulkanBackends);
	}

	VmaAllocator& getVmaAllocator()
	{
		return s_VmaAllocator;
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
				CHONPS_CORE_WARN("Validation Layer: {0}", pCallbackData->pMessage);
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

			if (s_VulkanBackends->enableValidationLayers)
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

		void createVmaAllocator()
		{
			VmaAllocatorCreateInfo allocatorInfo{};
			allocatorInfo.device = s_VulkanBackends->device;
			allocatorInfo.physicalDevice = s_VulkanBackends->physicalDevice;
			allocatorInfo.instance = s_VulkanBackends->instance;

			vmaCreateAllocator(&allocatorInfo, &s_VmaAllocator);
		}
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		CleanupSwapChain();

		if (m_VulkanBackends->bufferArray != VK_NULL_HANDLE)
			vkDestroyBuffer(m_VulkanBackends->device, m_VulkanBackends->bufferArray, nullptr);
		if (m_VulkanBackends->bufferArrayMemory != VK_NULL_HANDLE)
			vkFreeMemory(m_VulkanBackends->device, m_VulkanBackends->bufferArrayMemory, nullptr);


		vkDestroySampler(m_VulkanBackends->device, m_VulkanBackends->nullDataTexture.textureSampler, nullptr);
		vkDestroyImageView(m_VulkanBackends->device, m_VulkanBackends->nullDataTexture.textureImageView, nullptr);
		vkDestroyImage(m_VulkanBackends->device, m_VulkanBackends->nullDataTexture.textureImage, nullptr);
		vmaFreeMemory(s_VmaAllocator, m_VulkanBackends->nullDataTexture.textureImageMemory);

		vkDestroyDescriptorPool(m_VulkanBackends->device, m_VulkanBackends->descriptorPool, nullptr);

		vkDestroyDescriptorSetLayout(m_VulkanBackends->device, m_VulkanBackends->textureDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_VulkanBackends->device, m_VulkanBackends->frameBufferDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_VulkanBackends->device, m_VulkanBackends->cubemapDescriptorSetLayout, nullptr);

		for (auto& layout : m_VulkanBackends->descriptorSetLayouts)
			vkDestroyDescriptorSetLayout(m_VulkanBackends->device, layout, nullptr);

		for (size_t i = 0; i < m_VulkanBackends->maxFramesInFlight; i++)
		{
			vkDestroySemaphore(m_VulkanBackends->device, m_VulkanBackends->renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_VulkanBackends->device, m_VulkanBackends->imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_VulkanBackends->device, m_VulkanBackends->inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(m_VulkanBackends->device, m_VulkanBackends->commandPool, nullptr);

		vkDestroyRenderPass(m_VulkanBackends->device, m_VulkanBackends->renderPass, nullptr);

		vmaDestroyAllocator(s_VmaAllocator);

		vkDestroyDevice(m_VulkanBackends->device, nullptr);

		if (m_VulkanBackends->enableValidationLayers)
			vkUtils::DestroyDebugUtilsMessengerEXT(m_VulkanBackends->instance, m_VulkanBackends->debugMessenger, nullptr);

		vkDestroySurfaceKHR(m_VulkanBackends->instance, m_VulkanBackends->surface, nullptr);
		vkDestroyInstance(m_VulkanBackends->instance, nullptr);
	}

	void VulkanRendererAPI::Init()
	{
		m_VulkanBackends = std::make_shared<VulkanBackends>();
		s_VulkanBackends = m_VulkanBackends;
		vks::initStandardVulkanPipelineSpecification();

		uint32_t threadCount = std::thread::hardware_concurrency();
		m_ThreadPool.setThreadCount(threadCount);
		m_VulkanBackends->numThreads = threadCount;

		// Renderer Backends
		RendererBackends* rendererBackends = getRendererBackends();
		m_VulkanBackends->maxObjects = rendererBackends->maxRenderEntities;
		m_VulkanBackends->maxTextureBindingSlots = rendererBackends->maxTextureBindingSlots;
		m_VulkanBackends->maxFramesInFlight = rendererBackends->maxFramesInFlight;
		m_VulkanBackends->maxObjectIDs = rendererBackends->maxObjectIDs;
		m_VulkanBackends->multithreadingEnabled = rendererBackends->enableMultiThreading;
		m_VulkanBackends->enableValidationLayers = rendererBackends->vks.enableValidationLayers;

		s_TextureDescriptorBindings.textureBinding = rendererBackends->vks.textureBinding;
		s_TextureDescriptorBindings.samplerBinding = rendererBackends->vks.samplerBinding;
		s_TextureDescriptorBindings.frameBufferBinding = rendererBackends->vks.frameBufferBinding;
		s_TextureDescriptorBindings.cubemapBinding = rendererBackends->vks.cubemapBinding;

		PipelineSpecification pipelineSpec = getStandardPipelineSpecification();
		pipelineSpec.rasterizer.cullMode = rendererBackends->cullFaceMode;
		pipelineSpec.rasterizer.frontFace = rendererBackends->cullFrontFace;
		pipelineSpec.depthstencil.enableDepth = rendererBackends->enableDepthTesting;
		pipelineSpec.depthstencil.enableStencil = rendererBackends->enableStencilTesting;
		pipelineSpec.depthstencil.depthOpCompare = rendererBackends->depthOpCompare;

		setStandardPipelineSpecification(pipelineSpec);
		vks::fillQueueCountIDs(1, m_VulkanBackends->maxObjectIDs);

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

		vkUtils::createVmaAllocator();

		m_VulkanBackends->currentWindow = s_CurrentWindow;

		vkGetPhysicalDeviceProperties(m_VulkanBackends->physicalDevice, &m_VulkanBackends->deviceProperties);
		m_VulkanBackends->maxFrameBufferColorAttachments = m_VulkanBackends->deviceProperties.limits.maxColorAttachments;

		// Create Null texture if no texture is provided
		uint32_t textureData = 0x00000000;
		m_VulkanBackends->nullDataTexture = vks::createSingleDataTexture(1, 1, &textureData);
		m_VulkanBackends->texturesQueue.resize(m_VulkanBackends->maxFramesInFlight);

		// Create texture Layout
		// Create texture array Layout
		VkDescriptorSetLayoutBinding textureLayoutBinding{};
		textureLayoutBinding.binding = s_TextureDescriptorBindings.textureBinding;
		textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		textureLayoutBinding.descriptorCount = m_VulkanBackends->maxTextureBindingSlots;
		textureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		textureLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = s_TextureDescriptorBindings.samplerBinding;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		samplerLayoutBinding.descriptorCount = m_VulkanBackends->maxTextureBindingSlots;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = nullptr;

		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { textureLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
		layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		layoutInfo.pBindings = layoutBindings.data();

		CHONPS_CORE_ASSERT(vkCreateDescriptorSetLayout(m_VulkanBackends->device, &layoutInfo, nullptr, &m_VulkanBackends->textureDescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");

		// Create Offscreen FrameBuffer Layout
		VkDescriptorSetLayoutBinding frameBufferLayoutBinding{};
		frameBufferLayoutBinding.binding = s_TextureDescriptorBindings.frameBufferBinding;
		frameBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		frameBufferLayoutBinding.descriptorCount = m_VulkanBackends->maxFrameBufferColorAttachments;
		frameBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		frameBufferLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo FBlayoutInfo{};
		FBlayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		FBlayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
		FBlayoutInfo.bindingCount = 1;
		FBlayoutInfo.pBindings = &frameBufferLayoutBinding;

		CHONPS_CORE_ASSERT(vkCreateDescriptorSetLayout(m_VulkanBackends->device, &FBlayoutInfo, nullptr, &m_VulkanBackends->frameBufferDescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");

		// Create Cubemap Layout
		VkDescriptorSetLayoutBinding cubemapLayoutBinding{};
		cubemapLayoutBinding.binding = s_TextureDescriptorBindings.cubemapBinding;
		cubemapLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		cubemapLayoutBinding.descriptorCount = 1;
		cubemapLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		cubemapLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo cubemapLayoutInfo{};
		cubemapLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		cubemapLayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
		cubemapLayoutInfo.bindingCount = 1;
		cubemapLayoutInfo.pBindings = &cubemapLayoutBinding;

		CHONPS_CORE_ASSERT(vkCreateDescriptorSetLayout(m_VulkanBackends->device, &cubemapLayoutInfo, nullptr, &m_VulkanBackends->cubemapDescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");

		m_VulkanBackends->textureDescriptorBindings = &s_TextureDescriptorBindings;

		m_VulkanBackends->nullImageInfos.resize(m_VulkanBackends->maxTextureBindingSlots);
		for (uint32_t i = 0; i < m_VulkanBackends->nullImageInfos.size(); i++)
		{
			m_VulkanBackends->nullImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_VulkanBackends->nullImageInfos[i].imageView = m_VulkanBackends->nullDataTexture.textureImageView;
			m_VulkanBackends->nullImageInfos[i].sampler = m_VulkanBackends->nullDataTexture.textureSampler;
		}
	}

	void VulkanRendererAPI::CreateInstance()
	{
		if (m_VulkanBackends->enableValidationLayers)
		{
			CHONPS_CORE_INFO("Vulkan validation layers enabled");
			CHONPS_CORE_ASSERT(vkUtils::checkValidationLayerSupport(), "validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "CHONPS Framework";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 0);
		appInfo.pEngineName = "CHONPS";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 3, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = vkUtils::getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();


		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (m_VulkanBackends->enableValidationLayers)
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
			CHONPS_CORE_LOG_ERROR(Vulkan-Instance, "Failed to Initialize Instance!");
	}

	bool VulkanRendererAPI::SetUpDebugMessenger()
	{
		if (!m_VulkanBackends->enableValidationLayers) return false;
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
				m_VulkanBackends->msaaSamples = vks::getMaxUsableSampleCount();
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

		vkGetPhysicalDeviceFeatures(device, &m_VulkanBackends->deviceFeatures);

		VkPhysicalDeviceFeatures2 supportedFeatures2{};
		supportedFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

		VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParamFeatures{};
		shaderDrawParamFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
		shaderDrawParamFeatures.shaderDrawParameters = VK_TRUE;
		shaderDrawParamFeatures.pNext = nullptr;

		supportedFeatures2.pNext = &shaderDrawParamFeatures;

		vkGetPhysicalDeviceFeatures2(device, &supportedFeatures2);

		return indices.complete() && extensionsSupported && swapChainAdequate && m_VulkanBackends->deviceFeatures.samplerAnisotropy;
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

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		createInfo.pNext = &indexingFeatures;

		if (m_VulkanBackends->enableValidationLayers)
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
		m_VulkanBackends->swapChainCount = imageCount;
	}

	void VulkanRendererAPI::CreateImageViews()
	{
		m_VulkanBackends->swapChainImageViews.resize(m_VulkanBackends->swapChainImages.size());

		for (size_t i = 0; i < m_VulkanBackends->swapChainImages.size(); i++)
			m_VulkanBackends->swapChainImageViews[i] = vks::createImageView(m_VulkanBackends->swapChainImages[i], m_VulkanBackends->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
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
		depthAttachment.format = vks::findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
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
		std::array<VkDescriptorPoolSize, 3> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = m_VulkanBackends->maxDescriptorCounts;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[1].descriptorCount = m_VulkanBackends->maxDescriptorCounts;
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[2].descriptorCount = m_VulkanBackends->maxDescriptorCounts;

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
		VkFormat depthFormat = vks::findDepthFormat();

		vks::createImage(s_VulkanBackends->swapChainExtent.width, s_VulkanBackends->swapChainExtent.height, 0, VK_SAMPLE_COUNT_1_BIT, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VulkanBackends->depthImage, m_VulkanBackends->depthImageMemory);
		m_VulkanBackends->depthImageView = vks::createImageView(m_VulkanBackends->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		vks::transitionImageLayout(m_VulkanBackends->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
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
		//ThreadData* thread = &m_ThreadData[threadIndex];

		//VkCommandBufferBeginInfo beginInfo{};
		//beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		//beginInfo.pInheritanceInfo = &inheritanceInfo;

		//VkCommandBuffer cmdBuffer = thread->commandBuffers[cmdBufferIndex];

		//vkBeginCommandBuffer(cmdBuffer, &beginInfo);

		//VkViewport viewport{};
		//viewport.x = 0.0f;
		//viewport.y = 0.0f;
		//viewport.width = static_cast<float>(m_VulkanBackends->swapChainExtent.width);
		//viewport.height = static_cast<float>(m_VulkanBackends->swapChainExtent.height);
		//viewport.minDepth = 0.0f;
		//viewport.maxDepth = 1.0f;
		//vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

		//VkRect2D scissor{};
		//scissor.offset = { 0, 0 };
		//scissor.extent = m_VulkanBackends->swapChainExtent;
		//vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

		//VertexArray* lastVertexArray = nullptr;
		//uint32_t lastShaderIndex = 0; bool firstTimeBindShader = true;

		//for (auto& drawCommandData : thread->drawCommandData)
		//{
		//	DrawCommand drawCmd = drawCommandData.drawCmd;

		//	if (drawCmd.shaderIndex != lastShaderIndex || firstTimeBindShader == true)
		//	{
		//		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanBackends->graphicsPipeline[drawCmd.shaderIndex].pipeline);
		//		firstTimeBindShader = false;
		//	}
		//	lastShaderIndex = drawCmd.shaderIndex;

		//	if (drawCmd.vertexArray != lastVertexArray)
		//	{
		//		VkDeviceSize offsets[] = { 0 };
		//		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &drawCommandData.vertexBuffer, offsets);
		//		vkCmdBindIndexBuffer(cmdBuffer, drawCommandData.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		//	}
		//	lastVertexArray = drawCmd.vertexArray;

		//	// vkCmdPushConstants(cmdBuffer, m_VulkanBackends->graphicsPipeline[drawCmd.shaderIndex].pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantTextureIndexData), &m_VulkanBackends->textureIndexConstant);

		//	uint32_t indexCount = drawCmd.vertexArray->GetIndexCount();
		//	vkCmdDrawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0); // main draw call
		//}

		//vkEndCommandBuffer(cmdBuffer);
	}

	void VulkanRendererAPI::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t count)
	{
		std::vector<VkCommandBuffer> secondaryCommandBuffers;

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
				//secondaryCommandBuffers.push_back(m_ThreadData[t].commandBuffers[m_VulkanBackends->currentFrame]);
			}

			// Execute render commands from the secondary command buffer
			vkCmdExecuteCommands(commandBuffer, static_cast<uint32_t>(secondaryCommandBuffers.size()), secondaryCommandBuffers.data());
		}
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

	void VulkanRendererAPI::CreateOrUpdateTextureImage(uint32_t currentImage)
	{
		/*while (!m_VulkanBackends->texturesQueue[currentImage].empty())
		{
			uint32_t texID = m_VulkanBackends->texturesQueue[currentImage].front();
			VulkanTextureData texData = m_VulkanBackends->textureImages[texID];

			m_VulkanBackends->imageInfos[texID].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_VulkanBackends->imageInfos[texID].imageView = texData.textureImageView;
			m_VulkanBackends->imageInfos[texID].sampler = texData.textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_VulkanBackends->samplerDescriptorSet[currentImage];
			descriptorWrites[0].dstBinding = m_VulkanBackends->textureDescriptorBindings->textureBinding;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			descriptorWrites[0].descriptorCount = m_VulkanBackends->maxTextures;
			descriptorWrites[0].pImageInfo = m_VulkanBackends->imageInfos.data();

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_VulkanBackends->samplerDescriptorSet[currentImage];
			descriptorWrites[1].dstBinding = m_VulkanBackends->textureDescriptorBindings->samplerBinding;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			descriptorWrites[1].descriptorCount = m_VulkanBackends->maxTextures;
			descriptorWrites[1].pImageInfo = m_VulkanBackends->imageInfos.data();

			vkUpdateDescriptorSets(m_VulkanBackends->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

			m_VulkanBackends->texturesQueue[currentImage].pop();
		}*/
	}

	void VulkanRendererAPI::ClearColor(const float r, const float g, const float b, const float w)
	{
		m_VulkanBackends->clearColor = { {{r, g, b, w}} };
	}

	void VulkanRendererAPI::Clear()
	{

	}

	void VulkanRendererAPI::Draw(uint32_t vertexCount)
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		vkCmdDraw(m_VulkanBackends->commandBuffers[m_VulkanBackends->maxFramesInFlight], vertexCount, 1, 0, 0);
	}

	void VulkanRendererAPI::DrawIndexed(uint32_t indexCount)
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		vkCmdDrawIndexed(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], indexCount, 1, 0, 0, 0); // main draw call
	}

	void VulkanRendererAPI::DrawIndexed(VertexArray* vertexArray)
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		uint32_t indexCount = vertexArray->GetIndexCount();
		vkCmdDrawIndexed(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], indexCount, 1, 0, 0, 0); // main draw call
	}

	void VulkanRendererAPI::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		vkCmdDraw(m_VulkanBackends->commandBuffers[m_VulkanBackends->maxFramesInFlight], vertexCount, instanceCount, 0, firstInstance);
	}

	void VulkanRendererAPI::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		vkCmdDrawIndexed(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], indexCount, instanceCount, 0, 0, firstInstance); // main draw call
	}

	void VulkanRendererAPI::SetStencilReference(uint32_t reference)
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		vkCmdSetStencilReference(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], VK_STENCIL_FACE_FRONT_AND_BACK, reference);
	}

	void VulkanRendererAPI::SetStencilMask(uint32_t compareMask, uint32_t writeMask)
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		vkCmdSetStencilCompareMask(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], VK_STENCIL_FACE_FRONT_AND_BACK, compareMask);
		vkCmdSetStencilWriteMask(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], VK_STENCIL_FACE_FRONT_AND_BACK, writeMask);
	}

	void VulkanRendererAPI::BeginNextFrame()
	{

		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		// Clean up rendering data for next frame
		m_DrawList.clear();
		m_VulkanBackends->drawCallCount = 0;


		vkWaitForFences(m_VulkanBackends->device, 1, &m_VulkanBackends->inFlightFences[m_VulkanBackends->currentFrame], VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(m_VulkanBackends->device, m_VulkanBackends->swapChain, UINT64_MAX, m_VulkanBackends->imageAvailableSemaphores[m_VulkanBackends->currentFrame], VK_NULL_HANDLE, &m_VulkanBackends->imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		CHONPS_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image!");

		CreateOrUpdateTextureImage(m_VulkanBackends->currentFrame); // Update Descriptor Set Texture Array if new textures are added (new textures may override old textures with the same ID)

		// Only reset the fence if we are submitting work
		vkResetFences(m_VulkanBackends->device, 1, &m_VulkanBackends->inFlightFences[m_VulkanBackends->currentFrame]);

		vkResetCommandBuffer(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		CHONPS_CORE_ASSERT(vkBeginCommandBuffer(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], &beginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");
	}

	void VulkanRendererAPI::DrawSubmit()
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		m_VulkanBackends->drawCallCountTotal = m_VulkanBackends->drawCallCount;

		CHONPS_CORE_ASSERT(vkEndCommandBuffer(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame]) == VK_SUCCESS, "Failed to record command buffer!");

		// Submit render to GPU
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

	void VulkanRendererAPI::RenderPassBegin()
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_VulkanBackends->renderPass;
		renderPassInfo.framebuffer = m_VulkanBackends->swapChainFramebuffers[m_VulkanBackends->imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_VulkanBackends->swapChainExtent;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = m_VulkanBackends->clearColor.color;
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(m_VulkanBackends->swapChainExtent.height);
		viewport.width = static_cast<float>(m_VulkanBackends->swapChainExtent.width);
		viewport.height = -static_cast<float>(m_VulkanBackends->swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_VulkanBackends->swapChainExtent;
		vkCmdSetScissor(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], 0, 1, &scissor);

		vkCmdBeginRenderPass(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], &renderPassInfo, (m_VulkanBackends->multithreadingEnabled && m_VulkanBackends->drawCallCount >= m_VulkanBackends->numThreads) ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRendererAPI::RenderPassEnd()
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		vkCmdEndRenderPass(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame]);
	}

	void VulkanRendererAPI::PushConstant(uint32_t size, uint32_t offset, ShaderStage shaderStage, const void* pValues)
	{
		VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_ALL;
		switch (shaderStage)
		{
			case ShaderStage::Vertex:
			{
				shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			}
			case ShaderStage::Fragment:
			{
				shaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			}
			case ShaderStage::All:
			{
				shaderStageFlags = VK_SHADER_STAGE_ALL;
				break;
			}
		}

		vkCmdPushConstants(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], m_VulkanBackends->graphicsPipeline[m_VulkanBackends->currentBindedGraphicsPipeline].pipelineLayout, shaderStageFlags, offset, size, &pValues);
	}

	void VulkanRendererAPI::BindBufferSet(Shader* shader, UniformBuffer* buffer, uint32_t setIndex)
	{
		glfwGetFramebufferSize(s_CurrentWindow, &m_VulkanBackends->windowWidth, &m_VulkanBackends->windowHeight);
		if (m_VulkanBackends->windowWidth == 0 || m_VulkanBackends->windowHeight == 0)
			return;

		CHONPS_CORE_ASSERT(shader != nullptr, "Shader was nullptr!");
		uint32_t shaderID = shader->id();

		if (buffer != nullptr)
			vkCmdBindDescriptorSets(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanBackends->graphicsPipeline[shaderID].pipelineLayout, setIndex, 1, &m_VulkanBackends->bufferDescriptorSets[buffer->id()][m_VulkanBackends->currentFrame], 0, nullptr);
	}

	namespace vks
	{
		VkVertexInputAttributeDescription getAttributeDescriptions(uint32_t layout, VkFormat type, uint32_t stride, void* offset)
		{
			VkVertexInputAttributeDescription attributeDescriptions{};
			attributeDescriptions.binding = 0;
			attributeDescriptions.location = layout;
			attributeDescriptions.format = type;
			attributeDescriptions.offset = static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(offset));

			return attributeDescriptions;
		}
		
		VkVertexInputBindingDescription getBindingDescription(uint32_t stride)
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = stride;
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
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
			return 0;
		}

		void createBuffer(VkDeviceSize size, VkBuffer& buffer, VmaAllocation& bufferMemory, VkBufferUsageFlags usage, VmaMemoryUsage memUsage)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo allocInfo{};
			allocInfo.usage = memUsage;

			CHONPS_CORE_ASSERT(vmaCreateBuffer(s_VmaAllocator, &bufferInfo, &allocInfo, &buffer, &bufferMemory, nullptr) == VK_SUCCESS, "Failed to create Buffer!");
		}

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
		{
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			copyRegion.srcOffset = srcOffset;
			copyRegion.dstOffset = dstOffset;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			endSingleTimeCommands(commandBuffer);
		}

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkAccessFlags srcMask, VkAccessFlags dstMask)
		{
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkBufferMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			barrier.buffer = dstBuffer;
			barrier.size = size;
			barrier.offset = dstOffset;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.srcAccessMask = srcMask;
			barrier.dstAccessMask = dstMask;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &barrier, 0, nullptr);

			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			copyRegion.srcOffset = srcOffset;
			copyRegion.dstOffset = dstOffset;
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

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layerCount)
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
			barrier.subresourceRange.layerCount = layerCount;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (vks::hasStencilComponent(format))
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

		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount)
		{
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkBufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = layerCount;

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

		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
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
			imageInfo.samples = numSamples;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			CHONPS_CORE_ASSERT(vkCreateImage(s_VulkanBackends->device, &imageInfo, nullptr, &image) == VK_SUCCESS, "failed to create image!");

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(s_VulkanBackends->device, image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

			CHONPS_CORE_ASSERT(vkAllocateMemory(s_VulkanBackends->device, &allocInfo, nullptr, &imageMemory) == VK_SUCCESS, "failed to allocate image memory!");

			vkBindImageMemory(s_VulkanBackends->device, image, imageMemory, 0);
		}

		VulkanTextureData createSingleDataTexture(uint32_t width, uint32_t height, void* data)
		{
			VulkanTextureData vulkanTexture;

			VulkanBackends* m_VulkanBackends = getVulkanBackends();

			VkFormat format = renderGetGammaCorrection() == true ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;

			VkBuffer stagingBuffer;
			VmaAllocation stagingBufferMemory;

			VkDeviceSize imageSize = width * height * 4;

			vks::createBuffer(imageSize, stagingBuffer, stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

			void* bufferData;
			vmaMapMemory(s_VmaAllocator, stagingBufferMemory, &bufferData);
			memcpy(bufferData, data, static_cast<size_t>(imageSize));
			vmaUnmapMemory(s_VmaAllocator, stagingBufferMemory);

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

			VmaAllocationCreateInfo allocInfo{};
			allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

			vmaAllocateMemory(s_VmaAllocator, &memRequirements, &allocInfo, &vulkanTexture.textureImageMemory, nullptr);
			vmaBindImageMemory(s_VmaAllocator, vulkanTexture.textureImageMemory, vulkanTexture.textureImage);

			vks::transitionImageLayout(vulkanTexture.textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			vks::copyBufferToImage(stagingBuffer, vulkanTexture.textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

			vks::transitionImageLayout(vulkanTexture.textureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			vkDestroyBuffer(m_VulkanBackends->device, stagingBuffer, nullptr);
			vmaFreeMemory(s_VmaAllocator, stagingBufferMemory);

			vulkanTexture.textureImageView = vks::createImageView(vulkanTexture.textureImage, format, VK_IMAGE_ASPECT_COLOR_BIT);

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

		void fillQueueCountIDs(uint32_t firstElement, uint32_t finalElement)
		{
			s_VulkanBackends->uniformBufferCountIDs = QueueChain<uint32_t>(firstElement, finalElement);
			s_VulkanBackends->shaderCountIDs = QueueChain<uint32_t>(firstElement, finalElement);
			s_VulkanBackends->vertexBufferCountIDs = QueueChain<uint32_t>(firstElement, finalElement);
			s_VulkanBackends->indexBufferCountIDs = QueueChain<uint32_t>(firstElement, finalElement);
			s_VulkanBackends->vertexArrayBufferCountIDs = QueueChain<uint32_t>(firstElement, finalElement);
			s_VulkanBackends->textureCountIDs = QueueChain<uint32_t>(firstElement, finalElement);
			s_VulkanBackends->frameBufferCountIDs = QueueChain<uint32_t>(firstElement, finalElement);
		}

		VkFormat findDepthFormat()
		{
			return vkUtils::findSupportedFormat({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		}

		bool hasStencilComponent(VkFormat format)
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

		void vkImplTextureBinding(uint32_t textureBinding, uint32_t samplerBinding, uint32_t frameBufferBinding, uint32_t cubemapBinding)
		{
			CHONPS_CORE_ASSERT(textureBinding != samplerBinding, "textureBinding and samplerBinding must have a different value!");
			VulkanBackends* m_VulkanBackends = getVulkanBackends();

			s_TextureDescriptorBindings.textureBinding = textureBinding;
			s_TextureDescriptorBindings.samplerBinding = samplerBinding;
			s_TextureDescriptorBindings.frameBufferBinding = frameBufferBinding;
			s_TextureDescriptorBindings.cubemapBinding = cubemapBinding;
		}

		VkSampleCountFlagBits getMaxUsableSampleCount()
		{
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(s_VulkanBackends->physicalDevice, &physicalDeviceProperties);

			VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
			if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
			if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
			if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
			if (counts & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT; }
			if (counts & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT; }
			if (counts & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT; }

			return VK_SAMPLE_COUNT_1_BIT;
		}

		VkSampleCountFlagBits getSampleCountFlagBits(Sample samples)
		{
			switch (samples)
			{
				case Chonps::Sample::SampleCount_1_Bit: { return VK_SAMPLE_COUNT_1_BIT; }
				case Chonps::Sample::SampleCount_2_Bit: { return VK_SAMPLE_COUNT_2_BIT; }
				case Chonps::Sample::SampleCount_4_Bit: { return VK_SAMPLE_COUNT_4_BIT; }
				case Chonps::Sample::SampleCount_8_Bit: { return VK_SAMPLE_COUNT_8_BIT; }
				case Chonps::Sample::SampleCount_16_Bit: { return VK_SAMPLE_COUNT_16_BIT; }
				case Chonps::Sample::SampleCount_32_Bit: { return VK_SAMPLE_COUNT_32_BIT; }
				case Chonps::Sample::SampleCount_64_Bit: { return VK_SAMPLE_COUNT_64_BIT; }
				case Chonps::Sample::SampleCount_Max_Bit: { return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM; }
				default:
				{
					CHONPS_CORE_LOG_WARN(Vulkan, "Could not find sample, returning default sample!");
					return VK_SAMPLE_COUNT_1_BIT;
				}
			}
		}

		uint32_t getSampleCountValue(VkSampleCountFlagBits samples)
		{
			switch (samples)
			{
				case VK_SAMPLE_COUNT_1_BIT: { return 1; }
				case VK_SAMPLE_COUNT_2_BIT: { return 2; }
				case VK_SAMPLE_COUNT_4_BIT: { return 4; }
				case VK_SAMPLE_COUNT_8_BIT: { return 8; }
				case VK_SAMPLE_COUNT_16_BIT: { return 16; }
				case VK_SAMPLE_COUNT_32_BIT: { return 32; }
				case VK_SAMPLE_COUNT_64_BIT: { return 64; }
				case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM: { return 0x7FFFFFFF; }
				default:
				{
					CHONPS_CORE_LOG_WARN(Vulkan, "Could not find sample match!");
					return 1;
				}
			}
		}

		VkSampleCountFlagBits getSupportedSampleCount(Sample samples)
		{
			VkSampleCountFlagBits fbSampleCount = getSampleCountFlagBits(samples);
			VkSampleCountFlagBits maxSampleCount = vks::getMaxUsableSampleCount();

			uint32_t fbCount = getSampleCountValue(fbSampleCount);
			uint32_t maxCount = getSampleCountValue(maxSampleCount);

			if (fbCount > maxCount)
			{
				CHONPS_CORE_LOG_WARN(Vulkan, "Specified Sample count ({0}) is higher than the max sample count that is supported by the device ({1}); Using supported sample count instead!", fbCount, maxCount);
				return maxSampleCount;
			}
			else
				return fbSampleCount;
		}

		VkPrimitiveTopology getVulkanTopologyType(RenderTopologyType topologyType)
		{
			switch (topologyType)
			{
				case RenderTopologyType::Point: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
				case RenderTopologyType::Line: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
				case RenderTopologyType::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
				case RenderTopologyType::Triangle: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				case RenderTopologyType::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
				case RenderTopologyType::None: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			}

			CHONPS_CORE_LOG_ERROR(Vulkan, "No topology was chosen, cannot get topology type!");
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}

		VkCullModeFlags getVulkanCullModeFlag(RenderCullFaceMode cullFaceMode)
		{
			switch (cullFaceMode)
			{
				case RenderCullFaceMode::Disable: return VK_CULL_MODE_NONE;
				case RenderCullFaceMode::Front: return VK_CULL_MODE_FRONT_BIT;
				case RenderCullFaceMode::Back: return VK_CULL_MODE_BACK_BIT;
				case RenderCullFaceMode::Both: return VK_CULL_MODE_FRONT_AND_BACK;
			}

			CHONPS_CORE_LOG_ERROR(Vulkan, "No cull face mode was chosen, cannot get cull face mode!");
			return VK_CULL_MODE_NONE;
		}

		VkFrontFace getVulkanCullFrontFace(RenderCullFrontFace cullFrontFace)
		{
			switch (cullFrontFace)
			{
				case RenderCullFrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
				case RenderCullFrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
			}

			CHONPS_CORE_LOG_ERROR(Vulkan, "No cull front face was chosen, cannot get cull front face!");
			return VK_FRONT_FACE_CLOCKWISE;
		}

		VkColorComponentFlags getColorComponents(ColorWriteMask colorMask)
		{
			VkColorComponentFlags colorComponentsFlag = 0;

			if (colorMask.colorComponent_R_Bit) colorComponentsFlag |= VK_COLOR_COMPONENT_R_BIT;
			if (colorMask.colorComponent_G_Bit) colorComponentsFlag |= VK_COLOR_COMPONENT_G_BIT;
			if (colorMask.colorComponent_B_Bit) colorComponentsFlag |= VK_COLOR_COMPONENT_B_BIT;
			if (colorMask.colorComponent_A_Bit) colorComponentsFlag |= VK_COLOR_COMPONENT_A_BIT;

			return colorComponentsFlag;
		}

		VkBlendFactor getBlendFactor(ColorBlendFactor blendFactor)
		{
			switch (blendFactor)
			{
				case ColorBlendFactor::Zero: { return VK_BLEND_FACTOR_ZERO; }
				case ColorBlendFactor::One: { return VK_BLEND_FACTOR_ONE; }
				case ColorBlendFactor::SrcColor: { return VK_BLEND_FACTOR_SRC_COLOR; }
				case ColorBlendFactor::OneMinusSrcColor: { return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; }
				case ColorBlendFactor::DstColor: { return VK_BLEND_FACTOR_DST_COLOR; }
				case ColorBlendFactor::OneMinusDstColor: { return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; }
				case ColorBlendFactor::SrcAlpha: { return VK_BLEND_FACTOR_SRC_ALPHA; }
				case ColorBlendFactor::OneMinusSrcAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; }
				case ColorBlendFactor::DstAlpha: { return VK_BLEND_FACTOR_DST_ALPHA; }
				case ColorBlendFactor::OneMinusDstAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; }
				case ColorBlendFactor::ConstantColor: { return VK_BLEND_FACTOR_CONSTANT_COLOR; }
				case ColorBlendFactor::OneMinusConstantColor: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR; }
				case ColorBlendFactor::ConstantAlpha: { return VK_BLEND_FACTOR_CONSTANT_ALPHA; }
				case ColorBlendFactor::OneMinusConstantAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA; }
				case ColorBlendFactor::SrcAlphaSaturate: { return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; }
				case ColorBlendFactor::Src1Color: { return VK_BLEND_FACTOR_SRC1_COLOR; }
				case ColorBlendFactor::OneMinusSrc1Color: { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR; }
				case ColorBlendFactor::Src1_Alpha: { return VK_BLEND_FACTOR_SRC1_ALPHA; }
				case ColorBlendFactor::OneMinusSrc1Alpha: { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA; }
				default:
				{
					CHONPS_CORE_LOG_ERROR(Vulkan, "Could not find matching color blend factor!");
					return VK_BLEND_FACTOR_ZERO;
				}
			}
		}

		VkBlendOp getBlendOperation(ColorBlendOperation blendOp)
		{
			switch (blendOp)
			{
				case ColorBlendOperation::Add: { return VK_BLEND_OP_ADD; }
				case ColorBlendOperation::Subtract: { return VK_BLEND_OP_SUBTRACT; }
				case ColorBlendOperation::ReverseSubtract: { return VK_BLEND_OP_REVERSE_SUBTRACT; }
				case ColorBlendOperation::Min: { return VK_BLEND_OP_MIN; }
				case ColorBlendOperation::Max: { return VK_BLEND_OP_MAX; }
				case ColorBlendOperation::ZeroExt: { return VK_BLEND_OP_ZERO_EXT; }
				case ColorBlendOperation::SrcExt: { return VK_BLEND_OP_SRC_EXT; }
				case ColorBlendOperation::DstExt: { return VK_BLEND_OP_DST_EXT; }
				case ColorBlendOperation::SrcOverExt: { return VK_BLEND_OP_SRC_OVER_EXT; }
				case ColorBlendOperation::DstOverExt: { return VK_BLEND_OP_DST_OVER_EXT; }
				case ColorBlendOperation::SrcInExt: { return VK_BLEND_OP_SRC_IN_EXT; }
				case ColorBlendOperation::DstInExt: { return VK_BLEND_OP_DST_IN_EXT; }
				case ColorBlendOperation::SrcOutExt: { return VK_BLEND_OP_SRC_OUT_EXT; }
				case ColorBlendOperation::DstOutExt: { return VK_BLEND_OP_DST_OUT_EXT; }
				case ColorBlendOperation::SrcAtopExt: { return VK_BLEND_OP_SRC_ATOP_EXT; }
				case ColorBlendOperation::DstAtopExt: { return VK_BLEND_OP_DST_ATOP_EXT; }
				case ColorBlendOperation::XorExt: { return VK_BLEND_OP_XOR_EXT; }
				case ColorBlendOperation::MultiplyExt: { return VK_BLEND_OP_MULTIPLY_EXT; }
				case ColorBlendOperation::ScreenExt: { return VK_BLEND_OP_SCREEN_EXT; }
				case ColorBlendOperation::OverlayExt: { return VK_BLEND_OP_OVERLAY_EXT; }
				case ColorBlendOperation::DarkenExt: { return VK_BLEND_OP_DARKEN_EXT; }
				case ColorBlendOperation::LightenExt: { return VK_BLEND_OP_LIGHTEN_EXT; }
				case ColorBlendOperation::ColorDodgeExt: { return VK_BLEND_OP_COLORDODGE_EXT; }
				case ColorBlendOperation::ColorBurnExt: { return VK_BLEND_OP_COLORBURN_EXT; }
				case ColorBlendOperation::HardLightExt: { return VK_BLEND_OP_HARDLIGHT_EXT; }
				case ColorBlendOperation::SoftLightExt: { return VK_BLEND_OP_SOFTLIGHT_EXT; }
				case ColorBlendOperation::DifferenceExt: { return VK_BLEND_OP_DIFFERENCE_EXT; }
				case ColorBlendOperation::ExclusionExt: { return VK_BLEND_OP_EXCLUSION_EXT; }
				case ColorBlendOperation::InvertExt: { return VK_BLEND_OP_INVERT_EXT; }
				case ColorBlendOperation::InvertRGBExt: { return VK_BLEND_OP_INVERT_RGB_EXT; }
				case ColorBlendOperation::LinearDodgeExt: { return VK_BLEND_OP_LINEARDODGE_EXT; }
				case ColorBlendOperation::LinearBurnExt: { return VK_BLEND_OP_LINEARBURN_EXT; }
				case ColorBlendOperation::VividLightExt: { return VK_BLEND_OP_VIVIDLIGHT_EXT; }
				case ColorBlendOperation::LinearLightExt: { return VK_BLEND_OP_LINEARLIGHT_EXT; }
				case ColorBlendOperation::PinLightExt: { return VK_BLEND_OP_PINLIGHT_EXT; }
				case ColorBlendOperation::HardMixExt: { return VK_BLEND_OP_HARDMIX_EXT; }
				case ColorBlendOperation::HSLHueExt: { return VK_BLEND_OP_HSL_HUE_EXT; }
				case ColorBlendOperation::HSLSaturationExt: { return VK_BLEND_OP_HSL_SATURATION_EXT; }
				case ColorBlendOperation::HSLColorExt: { return VK_BLEND_OP_HSL_COLOR_EXT; }
				case ColorBlendOperation::HSLLuminosityExt: { return VK_BLEND_OP_HSL_LUMINOSITY_EXT; }
				case ColorBlendOperation::PlusExt: { return VK_BLEND_OP_PLUS_EXT; }
				case ColorBlendOperation::PlusClampedExt: { return VK_BLEND_OP_PLUS_CLAMPED_EXT; }
				case ColorBlendOperation::PlusClampedAlphaExt: { return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT; }
				case ColorBlendOperation::PlusDarkerExt: { return VK_BLEND_OP_PLUS_DARKER_EXT; }
				case ColorBlendOperation::MinusExt: { return VK_BLEND_OP_MINUS_EXT; }
				case ColorBlendOperation::MinusClampedExt: { return VK_BLEND_OP_MINUS_CLAMPED_EXT; }
				case ColorBlendOperation::ContrastExt: { return VK_BLEND_OP_CONTRAST_EXT; }
				case ColorBlendOperation::InvertOVGExt: { return VK_BLEND_OP_INVERT_OVG_EXT; }
				case ColorBlendOperation::RedExt: { return VK_BLEND_OP_RED_EXT; }
				case ColorBlendOperation::GreenExt: { return VK_BLEND_OP_GREEN_EXT; }
				case ColorBlendOperation::BlueExt: { return VK_BLEND_OP_BLUE_EXT; }
				default:
				{
					CHONPS_CORE_LOG_ERROR(Vulkan, "Could not find matching blend operation!");
					return VK_BLEND_OP_ADD;
				}
			}
		}

		VkCompareOp getCompareOp(CompareOperation compare)
		{
			switch (compare)
			{
				case CompareOperation::Never: { return VK_COMPARE_OP_NEVER; }
				case CompareOperation::Less: { return VK_COMPARE_OP_LESS; }
				case CompareOperation::Equal: { return VK_COMPARE_OP_EQUAL; }
				case CompareOperation::LessOrEqual: { return VK_COMPARE_OP_LESS_OR_EQUAL; }
				case CompareOperation::Greater: { return VK_COMPARE_OP_GREATER; }
				case CompareOperation::NotEqual: { return VK_COMPARE_OP_NOT_EQUAL; }
				case CompareOperation::GreaterOrEqual: { return VK_COMPARE_OP_GREATER_OR_EQUAL; }
				case CompareOperation::Always: { return VK_COMPARE_OP_ALWAYS; }
				default:
				{
					CHONPS_CORE_LOG_ERROR(Vulkan, "Could not find matching compare operation!");
					return VK_COMPARE_OP_NEVER;
				}
			}
		}

		VkStencilOp getStencilOp(StencilOperation stencilOp)
		{
			switch (stencilOp)
			{
				case StencilOperation::Keep: { return VK_STENCIL_OP_KEEP; }
				case StencilOperation::Zero: { return VK_STENCIL_OP_ZERO; }
				case StencilOperation::Replace: { return VK_STENCIL_OP_REPLACE; }
				case StencilOperation::IncrementAndClamp: { return VK_STENCIL_OP_INCREMENT_AND_CLAMP; }
				case StencilOperation::DecrementAndClamp: { return VK_STENCIL_OP_DECREMENT_AND_CLAMP; }
				case StencilOperation::Invert: { return VK_STENCIL_OP_INVERT; }
				case StencilOperation::IncrementAndWrap: { return VK_STENCIL_OP_INCREMENT_AND_WRAP; }
				case StencilOperation::DecrementAndWrap: { return VK_STENCIL_OP_DECREMENT_AND_WRAP; }
				default:
				{
					CHONPS_CORE_LOG_ERROR(Vulkan, "Could not find matching stencil operation!");
					return VK_STENCIL_OP_KEEP;
				}
			}
		}
	}
}