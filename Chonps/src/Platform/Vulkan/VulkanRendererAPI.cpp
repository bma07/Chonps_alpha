#include "cepch.h"
#include "VulkanRendererAPI.h"

#include "Core/File.h"
#include "VulkanPipeline.h"
#include "VulkanVertexBuffer.h"

#include <optional>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
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
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		CleanupSwapChain();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(m_VulkanBackends->device, m_VulkanBackends->renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_VulkanBackends->device, m_VulkanBackends->imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_VulkanBackends->device, m_VulkanBackends->inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(m_VulkanBackends->device, m_VulkanBackends->commandPool, nullptr);

		vkDestroyRenderPass(m_VulkanBackends->device, m_VulkanBackends->renderPass, nullptr);
		//vkDestroyPipeline(m_VulkanBackends->device, m_VulkanBackends->graphicsPipeline, nullptr);
		//vkDestroyPipelineLayout(m_VulkanBackends->device, m_VulkanBackends->pipelineLayout, nullptr);

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

		CreateInstance();
		SetUpDebugMessenger();
		CreateSurface();
		SetPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandPool();
		CreateCommandBuffers();
		CreateSyncObjects();
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
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

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
		/*VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);*/

		QueueFamilyIndices indices = FindQueueFamilies(device);

		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.complete() && extensionsSupported && swapChainAdequate;
	}

	VkSurfaceFormatKHR VulkanRendererAPI::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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

		VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT deviceInputDynamicStateFeatures{};
		deviceInputDynamicStateFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT;
		deviceInputDynamicStateFeatures.vertexInputDynamicState = true;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.pNext = &deviceInputDynamicStateFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

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
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_VulkanBackends->swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_VulkanBackends->swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			CHONPS_CORE_ASSERT(vkCreateImageView(m_VulkanBackends->device, &createInfo, nullptr, &m_VulkanBackends->swapChainImageViews[i]) == VK_SUCCESS, "Failed to create image views!");
		}
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

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		CHONPS_CORE_ASSERT(vkCreateRenderPass(m_VulkanBackends->device, &renderPassInfo, nullptr, &m_VulkanBackends->renderPass) == VK_SUCCESS, "Failed to create render pass!");
	}

	void VulkanRendererAPI::CreateGraphicsPipeline()
	{
		//auto vertShaderCode = get_file_bin("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_vert.spv");
		//auto fragShaderCode = get_file_bin("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_frag.spv");

		//VkShaderModule vertShaderModule;
		//// Vertex File Loading
		//VkShaderModuleCreateInfo createInfoVert{};
		//createInfoVert.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		//createInfoVert.codeSize = vertShaderCode.size();
		//createInfoVert.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());
		//CHONPS_CORE_ASSERT(vkCreateShaderModule(m_VulkanBackends->device, &createInfoVert, nullptr, &vertShaderModule) == VK_SUCCESS, "Failed to create shader module!");

		//VkShaderModule fragShaderModule;
		//// Fragmemt File Loading
		//VkShaderModuleCreateInfo createInfoFrag{};
		//createInfoFrag.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		//createInfoFrag.codeSize = fragShaderCode.size();
		//createInfoFrag.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());
		//CHONPS_CORE_ASSERT(vkCreateShaderModule(m_VulkanBackends->device, &createInfoFrag, nullptr, &fragShaderModule) == VK_SUCCESS, "Failed to create shader module!");

		//VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		//vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		//vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		//vertShaderStageInfo.module = vertShaderModule;
		//vertShaderStageInfo.pName = "main";

		//VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		//fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		//fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		//fragShaderStageInfo.module = fragShaderModule;
		//fragShaderStageInfo.pName = "main";

		//VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		//VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		//vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		//vertexInputInfo.vertexBindingDescriptionCount = 0;
		//vertexInputInfo.vertexAttributeDescriptionCount = 0;



		//VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		//inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		//inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		//inputAssembly.primitiveRestartEnable = VK_FALSE;

		//VkViewport viewport{};
		//viewport.x = 0.0f;
		//viewport.y = 0.0f;
		//viewport.width = (float)m_VulkanBackends->swapChainExtent.width;
		//viewport.height = (float)m_VulkanBackends->swapChainExtent.height;
		//viewport.minDepth = 0.0f;
		//viewport.maxDepth = 1.0f;

		//VkRect2D scissor{};
		//scissor.offset = { 0, 0 };
		//scissor.extent = m_VulkanBackends->swapChainExtent;

		//std::vector<VkDynamicState> dynamicStates =
		//{
		//	VK_DYNAMIC_STATE_VIEWPORT,
		//	VK_DYNAMIC_STATE_SCISSOR
		//};

		//VkPipelineDynamicStateCreateInfo dynamicState{};
		//dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		//dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		//dynamicState.pDynamicStates = dynamicStates.data();

		//VkPipelineViewportStateCreateInfo viewportState{};
		//viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		//viewportState.viewportCount = 1;
		//viewportState.scissorCount = 1;

		//VkPipelineRasterizationStateCreateInfo rasterizer{};
		//rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		//rasterizer.depthClampEnable = VK_FALSE;
		//rasterizer.rasterizerDiscardEnable = VK_FALSE;
		//rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		//rasterizer.lineWidth = 1.0f;
		//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		//rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		//rasterizer.depthBiasEnable = VK_FALSE;
		//rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		//rasterizer.depthBiasClamp = 0.0f; // Optional
		//rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		//VkPipelineMultisampleStateCreateInfo multisampling{};
		//multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		//multisampling.sampleShadingEnable = VK_FALSE;
		//multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		//multisampling.minSampleShading = 1.0f; // Optional
		//multisampling.pSampleMask = nullptr; // Optional
		//multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		//multisampling.alphaToOneEnable = VK_FALSE; // Optional

		//VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		//colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		//colorBlendAttachment.blendEnable = VK_FALSE;
		//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
		//
		//VkPipelineColorBlendStateCreateInfo colorBlending{};
		//colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		//colorBlending.logicOpEnable = VK_FALSE;
		//colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		//colorBlending.attachmentCount = 1;
		//colorBlending.pAttachments = &colorBlendAttachment;
		//colorBlending.blendConstants[0] = 0.0f; // Optional
		//colorBlending.blendConstants[1] = 0.0f; // Optional
		//colorBlending.blendConstants[2] = 0.0f; // Optional
		//colorBlending.blendConstants[3] = 0.0f; // Optional

		//VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		//pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		//pipelineLayoutInfo.setLayoutCount = 0; // Optional
		//pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		//pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		//pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		//CHONPS_CORE_ASSERT(vkCreatePipelineLayout(m_VulkanBackends->device, &pipelineLayoutInfo, nullptr, &m_VulkanBackends->pipelineLayout) == VK_SUCCESS, "Failed to create pipeline layout!");

		//VkGraphicsPipelineCreateInfo pipelineInfo{};
		//pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		//pipelineInfo.stageCount = 2;
		//pipelineInfo.pStages = shaderStages;
		//pipelineInfo.pVertexInputState = &vertexInputInfo;
		//pipelineInfo.pInputAssemblyState = &inputAssembly;
		//pipelineInfo.pViewportState = &viewportState;
		//pipelineInfo.pRasterizationState = &rasterizer;
		//pipelineInfo.pMultisampleState = &multisampling;
		//pipelineInfo.pDepthStencilState = nullptr; // Optional
		//pipelineInfo.pColorBlendState = &colorBlending;
		//pipelineInfo.pDynamicState = &dynamicState;
		//pipelineInfo.layout = m_VulkanBackends->pipelineLayout;
		//pipelineInfo.renderPass = m_VulkanBackends->renderPass;
		//pipelineInfo.subpass = 0;
		//pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		//pipelineInfo.basePipelineIndex = -1; // Optional

		//CHONPS_CORE_ASSERT(vkCreateGraphicsPipelines(m_VulkanBackends->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_VulkanBackends->graphicsPipeline) == VK_SUCCESS, "Failed to create graphics pipeline!");
	}

	void VulkanRendererAPI::CreateFramebuffers()
	{
		m_VulkanBackends->swapChainFramebuffers.resize(m_VulkanBackends->swapChainImageViews.size());

		for (size_t i = 0; i < m_VulkanBackends->swapChainImageViews.size(); i++)
		{
			VkImageView attachments[] = { m_VulkanBackends->swapChainImageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_VulkanBackends->renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_VulkanBackends->swapChainExtent.width;
			framebufferInfo.height = m_VulkanBackends->swapChainExtent.height;
			framebufferInfo.layers = 1;

			CHONPS_CORE_ASSERT(vkCreateFramebuffer(m_VulkanBackends->device, &framebufferInfo, nullptr, &m_VulkanBackends->swapChainFramebuffers[i]) == VK_SUCCESS, "Failed to create framebuffer!");
		}
	}

	void VulkanRendererAPI::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_VulkanBackends->physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		CHONPS_CORE_ASSERT(vkCreateCommandPool(m_VulkanBackends->device, &poolInfo, nullptr, &m_VulkanBackends->commandPool) == VK_SUCCESS, "Failed to create command pool!");
	}

	void VulkanRendererAPI::CreateCommandBuffers()
	{
		m_VulkanBackends->commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_VulkanBackends->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)m_VulkanBackends->commandBuffers.size();

		CHONPS_CORE_ASSERT(vkAllocateCommandBuffers(m_VulkanBackends->device, &allocInfo, m_VulkanBackends->commandBuffers.data()) == VK_SUCCESS, "Failed to allocate command buffers!");
	}

	void VulkanRendererAPI::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const uint32_t& count)
	{
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

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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

		PFN_vkCmdSetVertexInputEXT CmdSetVertexInputEXT{};
		CmdSetVertexInputEXT = (PFN_vkCmdSetVertexInputEXT)vkGetInstanceProcAddr(m_VulkanBackends->instance, "vkCmdSetVertexInputEXT");
		CmdSetVertexInputEXT(commandBuffer, 1, &m_VulkanBackends->bindingDescriptions, m_VulkanBackends->attributeDescriptions.size(), m_VulkanBackends->attributeDescriptions.data());

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanBackends->graphicsPipeline);

		VkBuffer vertexBuffers[] = { m_VulkanBackends->vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_VulkanBackends->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		
		vkCmdDrawIndexed(commandBuffer, count, 1, 0, 0, 0); // main draw call

		vkCmdEndRenderPass(commandBuffer);

		CHONPS_CORE_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed to record command buffer!");
	}

	void VulkanRendererAPI::CreateSyncObjects()
	{
		m_VulkanBackends->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_VulkanBackends->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_VulkanBackends->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
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
		CreateFramebuffers();
	}

	void VulkanRendererAPI::CleanupSwapChain()
	{
		for (size_t i = 0; i < m_VulkanBackends->swapChainFramebuffers.size(); i++)
			vkDestroyFramebuffer(m_VulkanBackends->device, m_VulkanBackends->swapChainFramebuffers[i], nullptr);

		for (size_t i = 0; i < m_VulkanBackends->swapChainImageViews.size(); i++)
			vkDestroyImageView(m_VulkanBackends->device, m_VulkanBackends->swapChainImageViews[i], nullptr);

		vkDestroySwapchainKHR(m_VulkanBackends->device, m_VulkanBackends->swapChain, nullptr);
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
		int width, height;
		glfwGetFramebufferSize(s_CurrentWindow, &width, &height);
		if (width == 0 || height == 0)
			return;

		vkWaitForFences(m_VulkanBackends->device, 1, &m_VulkanBackends->inFlightFences[m_VulkanBackends->currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_VulkanBackends->device, m_VulkanBackends->swapChain, UINT64_MAX, m_VulkanBackends->imageAvailableSemaphores[m_VulkanBackends->currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		CHONPS_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image!");

		// Only reset the fence if we are submitting work
		vkResetFences(m_VulkanBackends->device, 1, &m_VulkanBackends->inFlightFences[m_VulkanBackends->currentFrame]);

		vkResetCommandBuffer(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
		RecordCommandBuffer(m_VulkanBackends->commandBuffers[m_VulkanBackends->currentFrame], imageIndex, count);

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
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(m_VulkanBackends->presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_VulkanBackends->framebufferResized)
		{
			m_VulkanBackends->framebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
			CHONPS_CORE_ASSERT(false, "Failed to present swap chain image!");

		m_VulkanBackends->currentFrame = (m_VulkanBackends->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; // Modulo (%) will loop the frame index back to beggining after every Max Frame
	}

	void VulkanRendererAPI::DrawLine()
	{

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

	void VulkanRendererAPI::UploadUniform1f(uint32_t shader, const char* uniform, float x)
	{

	}

	void VulkanRendererAPI::UploadUniform2f(uint32_t shader, const char* uniform, float x, float y)
	{

	}

	void VulkanRendererAPI::UploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z)
	{

	}

	void VulkanRendererAPI::UploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w)
	{

	}

	void VulkanRendererAPI::UploadUniform1i(uint32_t shader, const char* uniform, int x)
	{

	}

	void VulkanRendererAPI::UploadUniform2i(uint32_t shader, const char* uniform, int x, int y)
	{

	}

	void VulkanRendererAPI::UploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z)
	{

	}

	void VulkanRendererAPI::UploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w)
	{

	}

	void VulkanRendererAPI::UploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x)
	{

	}

	void VulkanRendererAPI::UploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y)
	{

	}

	void VulkanRendererAPI::UploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z)
	{

	}

	void VulkanRendererAPI::UploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w)
	{

	}

	void VulkanRendererAPI::UploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{

	}

	void VulkanRendererAPI::UploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{

	}

	void VulkanRendererAPI::UploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{

	}

	void VulkanRendererAPI::UploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{

	}

	void VulkanRendererAPI::UploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{

	}

	void VulkanRendererAPI::UploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{

	}

	void VulkanRendererAPI::UploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{

	}

	void VulkanRendererAPI::UploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{

	}

	void VulkanRendererAPI::UploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{

	}

	void VulkanRendererAPI::UploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{

	}
}