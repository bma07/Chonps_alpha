#include "cepch.h"
#include "VulkanFrameBuffer.h"

#include "VulkanShader.h"

namespace Chonps
{
	float screenVkImpl[] =
	{
		// Coords    // texCoords	//	Tex Coords flipped due to Vulkan invert of y-axis
		-1.0f,  1.0f,  0.0f, 0.0f,  //	
		-1.0f, -1.0f,  0.0f, 1.0f,	//	(0,0)----(1,0)
		 1.0f, -1.0f,  1.0f, 1.0f,	//	  |	\	   |
									//	  |	  \	   |
		-1.0f,  1.0f,  0.0f, 0.0f,	//	  |		\  |
		 1.0f, -1.0f,  1.0f, 1.0f,	//  (0,1)----(1,1)
		 1.0f,  1.0f,  1.0f, 0.0f,	//
	};

	VkFormat getColorFormat(FrameBufferColorFormat texFormat)
	{
		switch (texFormat)
		{
			case FrameBufferColorFormat::None:
			{
				return VK_FORMAT_R8G8B8A8_UNORM;
			}
			case FrameBufferColorFormat::RGB:
			{
				return VK_FORMAT_R8G8B8_UNORM;
			}
			case FrameBufferColorFormat::RGBA:
			{
				return VK_FORMAT_R8G8B8A8_UNORM;
			}
			case FrameBufferColorFormat::RGBA8:
			{
				return VK_FORMAT_R8G8B8A8_UNORM;
			}
			case FrameBufferColorFormat::RGBA16F:
			{
				return VK_FORMAT_R16G16B16A16_SFLOAT;
			}
			case FrameBufferColorFormat::RGBA32F:
			{
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			case FrameBufferColorFormat::RED_INT:
			{
				return VK_FORMAT_R8_SINT;
			}
			default:
			{
				CHONPS_CORE_WARN("WARNING: FRAMEBUFFER: No options matched FrameBufferColorFormat that are available for color format, defaulting to standard texture format!");
				return VK_FORMAT_R8G8B8A8_UNORM;
			}
		}
	}

	VkFormat getDepthFormat(FrameBufferDepthFormat texFormat)
	{
		switch (texFormat)
		{
			case FrameBufferDepthFormat::DEPTH_COMPONENT:
			{
				return vks::findDepthFormat();
			}
			case FrameBufferDepthFormat::DEPTH24STENCIL8:
			{
				return VK_FORMAT_D24_UNORM_S8_UINT;
			}
			default:
			{
				CHONPS_CORE_WARN("WARNING: FRAMEBUFFER: No options matched FrameBufferDepthFormat that are available for depth format, defaulting to standard texture format!");
				return vks::findDepthFormat();
			}
		}
	}

	VkFilter getImageFilter(TexFilter filter)
	{
		switch (filter)
		{
			case Chonps::TexFilter::Linear:
			{
				return VK_FILTER_LINEAR;
			}
			case Chonps::TexFilter::Nearest:
			{
				return VK_FILTER_NEAREST;
			}
			default:
			{
				CHONPS_CORE_ERROR("ERROR: TEXTURE: Given texture filter not supported!");
				CHONPS_CORE_WARN("WARNING: TEXTURE: texture mag filter not found, using default filter instead!");
				return VK_FILTER_NEAREST;
			}
		}
	}

	VkSamplerAddressMode getAddressMode(TexWrap wrap)
	{
		switch (wrap)
		{
			case Chonps::TexWrap::Default:
			{
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			}
			case Chonps::TexWrap::Repeat:
			{
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			}
			case Chonps::TexWrap::MirroredRepeat:
			{
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			}
			case Chonps::TexWrap::ClampToEdge:
			{
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			}
			default:
			{
				CHONPS_CORE_ERROR("ERROR: TEXTURE: Given texture wrap not supported!");
				CHONPS_CORE_WARN("WARNING: TEXTURE: texture wrap not found, using default wrap instead!");
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			}
		}
	}

	void VulkanFrameBuffer::CreateFrameBuffer(VkRenderPass renderPass, VulkanFrameBufferData* frameBuf, FrameBufferSpecificationInfo specification)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		VkSampleCountFlagBits sampleCount = vks::getSupportedSampleCount(specification.samples);
		uint32_t attachmentCount = specification.colorAttachmentCount + (specification.depthAttachment != nullptr ? 1 : 0);

		std::vector<VkImageView> attachments; attachments.resize(attachmentCount);

		VmaAllocationCreateInfo memAlloc{};
		VkMemoryRequirements memReqs{};

		// Color attachment
		frameBuf->color.resize(attachmentCount);
		
		for (auto& attachment : m_ColorAttachments)
		{
			VkFormat colorFormat = getColorFormat(attachment.format);

			VkImageCreateInfo image{};
			image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image.imageType = VK_IMAGE_TYPE_2D;
			image.format = colorFormat;
			image.extent.width = specification.width;
			image.extent.height = specification.height;
			image.extent.depth = 1;
			image.mipLevels = 1;
			image.arrayLayers = 1;
			image.samples = sampleCount;
			image.tiling = VK_IMAGE_TILING_OPTIMAL;
			// We will sample directly from the color attachment
			image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			VkImageViewCreateInfo colorImageView{};
			colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorImageView.format = colorFormat;
			colorImageView.flags = 0;
			colorImageView.subresourceRange = {};
			colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorImageView.subresourceRange.baseMipLevel = 0;
			colorImageView.subresourceRange.levelCount = 1;
			colorImageView.subresourceRange.baseArrayLayer = 0;
			colorImageView.subresourceRange.layerCount = 1;

			CHONPS_CORE_ASSERT(attachment.index < frameBuf->color.size(), "Color Attachment index must be less than the total number of color attachments!");

			CHONPS_CORE_ASSERT(vkCreateImage(vkBackends->device, &image, nullptr, &frameBuf->color[attachment.index].image) == VK_SUCCESS, "Failed to create FrameBuffer image!");
			vkGetImageMemoryRequirements(vkBackends->device, frameBuf->color[attachment.index].image, &memReqs);
			memAlloc.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			memAlloc.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			CHONPS_CORE_ASSERT(vmaAllocateMemory(vmaAllocator, &memReqs, &memAlloc, &frameBuf->color[attachment.index].memory, nullptr) == VK_SUCCESS, "Failed to allocate FrameBuffer memory!");
			CHONPS_CORE_ASSERT(vmaBindImageMemory(vmaAllocator, frameBuf->color[attachment.index].memory, frameBuf->color[attachment.index].image) == VK_SUCCESS, "Failed to bind FrameBuffer image memory!");

			colorImageView.image = frameBuf->color[attachment.index].image;
			CHONPS_CORE_ASSERT(vkCreateImageView(vkBackends->device, &colorImageView, nullptr, &frameBuf->color[attachment.index].view) == VK_SUCCESS, "Failed to create FrameBuffer image view!");

			attachments[attachment.index] = frameBuf->color[attachment.index].view;
		}

		if (m_MultiSampled) // Multisampling
		{
			frameBuf->msaaColor.resize(m_ColorAttachments.size());
			for (auto& attachment : m_ColorAttachments)
			{
				VkFormat colorFormat = getColorFormat(attachment.format);

				VkImageCreateInfo image{};
				image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				image.imageType = VK_IMAGE_TYPE_2D;
				image.format = colorFormat;
				image.extent.width = specification.width;
				image.extent.height = specification.height;
				image.extent.depth = 1;
				image.mipLevels = 1;
				image.arrayLayers = 1;
				image.samples = VK_SAMPLE_COUNT_1_BIT;
				image.tiling = VK_IMAGE_TILING_OPTIMAL;
				// We will sample directly from the color attachment
				image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

				VkImageViewCreateInfo colorImageView{};
				colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
				colorImageView.format = colorFormat;
				colorImageView.flags = 0;
				colorImageView.subresourceRange = {};
				colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				colorImageView.subresourceRange.baseMipLevel = 0;
				colorImageView.subresourceRange.levelCount = 1;
				colorImageView.subresourceRange.baseArrayLayer = 0;
				colorImageView.subresourceRange.layerCount = 1;

				CHONPS_CORE_ASSERT(attachment.index < frameBuf->msaaColor.size(), "Color Attachment index must be less than the total number of color attachments!");

				CHONPS_CORE_ASSERT(vkCreateImage(vkBackends->device, &image, nullptr, &frameBuf->msaaColor[attachment.index].image) == VK_SUCCESS, "Failed to create FrameBuffer image!");
				vkGetImageMemoryRequirements(vkBackends->device, frameBuf->msaaColor[attachment.index].image, &memReqs);
				memAlloc.usage = VMA_MEMORY_USAGE_GPU_ONLY;
				memAlloc.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	

				CHONPS_CORE_ASSERT(vmaAllocateMemory(vmaAllocator, &memReqs, &memAlloc, &frameBuf->msaaColor[attachment.index].memory, nullptr) == VK_SUCCESS, "Failed to allocate FrameBuffer memory!");
				CHONPS_CORE_ASSERT(vmaBindImageMemory(vmaAllocator, frameBuf->msaaColor[attachment.index].memory, frameBuf->msaaColor[attachment.index].image) == VK_SUCCESS, "Failed to bind FrameBuffer image memory!");

				colorImageView.image = frameBuf->msaaColor[attachment.index].image;
				CHONPS_CORE_ASSERT(vkCreateImageView(vkBackends->device, &colorImageView, nullptr, &frameBuf->msaaColor[attachment.index].view) == VK_SUCCESS, "Failed to create FrameBuffer image view!");

				attachments.push_back(frameBuf->msaaColor[attachment.index].view);
			}
		}

		// Depth stencil attachment
		if (m_DepthAttachment.format != FrameBufferDepthFormat::None)
		{
			VkFormat depthFormat = getDepthFormat(m_DepthAttachment.format);

			VkImageCreateInfo image{};
			image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image.imageType = VK_IMAGE_TYPE_2D;
			image.extent.width = specification.width;
			image.extent.height = specification.height;
			image.extent.depth = 1;
			image.mipLevels = 1;
			image.arrayLayers = 1;
			image.samples = sampleCount;
			image.tiling = VK_IMAGE_TILING_OPTIMAL;

			image.format = depthFormat;
			image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			VkImageViewCreateInfo depthStencilView{};
			depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			depthStencilView.format = depthFormat;
			depthStencilView.flags = 0;
			depthStencilView.subresourceRange = {};
			depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (vks::hasStencilComponent(depthFormat))
				depthStencilView.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

			depthStencilView.subresourceRange.baseMipLevel = 0;
			depthStencilView.subresourceRange.levelCount = 1;
			depthStencilView.subresourceRange.baseArrayLayer = 0;
			depthStencilView.subresourceRange.layerCount = 1;

			CHONPS_CORE_ASSERT(vkCreateImage(vkBackends->device, &image, nullptr, &frameBuf->depth.image) == VK_SUCCESS, "Failed to create FrameBuffer image!");
			vkGetImageMemoryRequirements(vkBackends->device, frameBuf->depth.image, &memReqs);
			memAlloc.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			memAlloc.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;


			CHONPS_CORE_ASSERT(vmaAllocateMemory(vmaAllocator, &memReqs, &memAlloc, &frameBuf->depth.memory, nullptr) == VK_SUCCESS, "Failed to allocate FrameBuffer memory!");
			CHONPS_CORE_ASSERT(vmaBindImageMemory(vmaAllocator, frameBuf->depth.memory, frameBuf->depth.image) == VK_SUCCESS, "Failed to bind FrameBuffer image memory!");

			depthStencilView.image = frameBuf->depth.image;
			CHONPS_CORE_ASSERT(vkCreateImageView(vkBackends->device, &depthStencilView, nullptr, &frameBuf->depth.view) == VK_SUCCESS, "Failed to create FrameBuffer image view!");

			CHONPS_CORE_ASSERT(m_DepthAttachment.index < attachmentCount, "Depth Attachment index must be less than the total number of attachments!");
			attachments[m_DepthAttachment.index] = frameBuf->depth.view;
		}

		VkFramebufferCreateInfo fbufCreateInfo{};
		fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufCreateInfo.renderPass = renderPass;
		fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		fbufCreateInfo.pAttachments = attachments.data();
		fbufCreateInfo.width = specification.width;
		fbufCreateInfo.height = specification.height;
		fbufCreateInfo.layers = 1;

		CHONPS_CORE_ASSERT(vkCreateFramebuffer(vkBackends->device, &fbufCreateInfo, nullptr, &frameBuf->framebuffer) == VK_SUCCESS, "Failed to create FrameBuffer!");
	}

	void VulkanFrameBuffer::UpdateFrameBufferImage(uint32_t currentImage)
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		std::vector<VkDescriptorImageInfo> imageInfos; imageInfos.resize(vkBackends->maxFrameBufferColorAttachments);
		for (auto& attachment : m_ColorAttachments)
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_MultiSampled ? m_OffscreenPass.framebuffer.msaaColor[attachment.index].view : m_OffscreenPass.framebuffer.color[attachment.index].view;
			imageInfo.sampler = m_OffscreenPass.sampler;

			imageInfos[attachment.index] = imageInfo;
		}
		for (auto& imageInfo : imageInfos)
		{
			if (imageInfo.imageView == VK_NULL_HANDLE)
			{
				VulkanTextureData texData = vkBackends->nullDataTexture;

				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = texData.textureImageView;
				imageInfo.sampler = m_OffscreenPass.sampler;
			}
		}

		VkWriteDescriptorSet  descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_DescriptorSets[currentImage];
		descriptorWrite.dstBinding = vkBackends->textureDescriptorBindings->frameBufferBinding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = vkBackends->maxFrameBufferColorAttachments;
		descriptorWrite.pImageInfo = imageInfos.data();

		vkUpdateDescriptorSets(vkBackends->device, 1, &descriptorWrite, 0, nullptr);
	}

	VulkanFrameBuffer::VulkanFrameBuffer(FrameBufferSpecificationInfo frameBufferSpecificationInfo, uint32_t setIndex)
		: FrameBuffer(frameBufferSpecificationInfo, setIndex), m_FrameBufferSpecificationInfo(frameBufferSpecificationInfo), m_SetIndex(setIndex)
	{
		m_OffscreenPass.width = frameBufferSpecificationInfo.width;
		m_OffscreenPass.height = frameBufferSpecificationInfo.height;

		m_ColorAttachments = std::vector<FrameBufferColorAttachment>(frameBufferSpecificationInfo.colorAttachments, frameBufferSpecificationInfo.colorAttachments + frameBufferSpecificationInfo.colorAttachmentCount);
		m_DepthAttachment = *frameBufferSpecificationInfo.depthAttachment;

		m_VPx = 0;
		m_VPy = 0;
		m_VPWidth = frameBufferSpecificationInfo.width;
		m_VPHeight = frameBufferSpecificationInfo.height;

		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		m_ID = vkBackends->frameBufferCountIDs.take_next();
		m_TexID = vkBackends->textureCountIDs.take_next();

		VkSampleCountFlagBits sampleCount = vks::getSupportedSampleCount(frameBufferSpecificationInfo.samples);
		m_MultiSampled = !(sampleCount & VK_SAMPLE_COUNT_1_BIT);

		uint32_t attachmentCount = frameBufferSpecificationInfo.colorAttachmentCount + (frameBufferSpecificationInfo.depthAttachment != nullptr ? 1 : 0);
		CHONPS_CORE_ASSERT(attachmentCount <= vkBackends->maxFrameBufferColorAttachments, "FrameBuffer must have less attachments than the max attachments defined in the renderer API!");

		std::vector<VkAttachmentDescription> attachmentDescriptions; attachmentDescriptions.resize(attachmentCount);
		m_ClearValues.resize(attachmentCount);

		std::vector<VkAttachmentReference> colorReference;
		VkAttachmentReference depthReference;

		// Color attachments
		for (uint32_t i = 0; i < frameBufferSpecificationInfo.colorAttachmentCount; i++)
		{
			FrameBufferColorAttachment colorAttachment = frameBufferSpecificationInfo.colorAttachments[i];

			VkAttachmentDescription attchmentDescription{};
			attchmentDescription.format = getColorFormat(colorAttachment.format);
			attchmentDescription.samples = sampleCount;
			attchmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attchmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attchmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attchmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attchmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attchmentDescription.finalLayout = m_MultiSampled ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			CHONPS_CORE_ASSERT(colorAttachment.index < attachmentCount, "Attachment Index must be less than the total number of attachments!");
			if (frameBufferSpecificationInfo.depthAttachment != nullptr)
				CHONPS_CORE_ASSERT(colorAttachment.index != frameBufferSpecificationInfo.depthAttachment->index, "Color Attachment Index must not be the same to Depth Attachment Index!");

			attachmentDescriptions[colorAttachment.index] = attchmentDescription;
			m_ClearValues[colorAttachment.index].color = vkBackends->clearColor.color;
			colorReference.push_back({ colorAttachment.index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
		}

		// Depth attachment
		if (frameBufferSpecificationInfo.depthAttachment != nullptr)
		{
			VkAttachmentDescription attchmentDescription{};
			attchmentDescription.format = getDepthFormat(frameBufferSpecificationInfo.depthAttachment->format);
			attchmentDescription.samples = sampleCount;
			attchmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attchmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attchmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attchmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attchmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attchmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			CHONPS_CORE_ASSERT(frameBufferSpecificationInfo.depthAttachment->index < attachmentCount, "Attachment Index must be less than the total number of attachments!");
			attachmentDescriptions[frameBufferSpecificationInfo.depthAttachment->index] = attchmentDescription;
			m_ClearValues[frameBufferSpecificationInfo.depthAttachment->index].depthStencil = { 1.0f, 0 };
			m_DepthAttached = true;
			depthReference = { frameBufferSpecificationInfo.depthAttachment->index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		}

		// Multisample
		std::vector<VkAttachmentReference> attachReferenceResolves;
		if (m_MultiSampled)
		{
			size_t attachmentCount = attachmentDescriptions.size();
			for (uint32_t i = 0; i < attachmentCount; i++)
			{
				if (frameBufferSpecificationInfo.depthAttachment != nullptr && attachmentDescriptions[i].format == getDepthFormat(frameBufferSpecificationInfo.depthAttachment->format))
					continue;

				VkAttachmentDescription attachmentResolve{};
				attachmentResolve.format = attachmentDescriptions[i].format;
				attachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
				attachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				attachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				attachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				attachmentDescriptions.push_back(attachmentResolve);

				VkAttachmentReference attachResolve{};
				attachResolve.attachment = static_cast<uint32_t>(attachmentCount + i);
				attachResolve.layout = attachmentDescriptions[i].finalLayout;
				attachReferenceResolves.push_back(attachResolve);
			}
		}

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorReference.size());
		subpassDescription.pColorAttachments = colorReference.data();
		subpassDescription.pDepthStencilAttachment = &depthReference;
		subpassDescription.pResolveAttachments = attachReferenceResolves.data();

		// Use subpass dependencies for layout transitions
		std::array<VkSubpassDependency, 2> dependencies{};

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		// Create the actual renderpass
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
		renderPassInfo.pAttachments = attachmentDescriptions.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		CHONPS_CORE_ASSERT(vkCreateRenderPass(vkBackends->device, &renderPassInfo, nullptr, &m_OffscreenPass.renderPass) == VK_SUCCESS, "Failed to  create FrameBuffer render pass!");

		VkFilter filter = getImageFilter(frameBufferSpecificationInfo.textureFilter);
		VkSamplerAddressMode addressMode = getAddressMode(frameBufferSpecificationInfo.textureWrap);

		// Create sampler to sample from the color attachments
		VkSamplerCreateInfo sampler{};
		sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler.magFilter = filter;
		sampler.minFilter = filter;
		sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler.addressModeU = addressMode;
		sampler.addressModeV = addressMode;
		sampler.addressModeW = addressMode;
		sampler.mipLodBias = 0.0f;
		sampler.maxAnisotropy = 1.0f;
		sampler.minLod = 0.0f;
		sampler.maxLod = 1.0f;
		sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		CHONPS_CORE_ASSERT(vkCreateSampler(vkBackends->device, &sampler, nullptr, &m_OffscreenPass.sampler) == VK_SUCCESS, "Failed to FrameBuffer");

		// Create FrameBuffer
		CreateFrameBuffer(m_OffscreenPass.renderPass, &m_OffscreenPass.framebuffer, frameBufferSpecificationInfo);

		// Create Vertex Buffer
		VkDeviceSize bufferSize = sizeof(screenVkImpl);

		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferMemory;

		vks::createBuffer(bufferSize, stagingBuffer, stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* data;
		vmaMapMemory(vmaAllocator, stagingBufferMemory, &data);
		std::memcpy(data, screenVkImpl, (size_t)bufferSize);
		vmaUnmapMemory(vmaAllocator, stagingBufferMemory);

		vks::createBuffer(bufferSize, m_VertexBuffer, m_VertexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		vks::copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

		vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, stagingBufferMemory);

		// Allocate Descriptor Set
		std::vector<VkDescriptorSetLayout> frameBufferLayouts(vkBackends->maxFramesInFlight, vkBackends->frameBufferDescriptorSetLayout);
		VkDescriptorSetAllocateInfo FBsamplerAllocInfo{};
		FBsamplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		FBsamplerAllocInfo.descriptorPool = vkBackends->descriptorPool;
		FBsamplerAllocInfo.descriptorSetCount = static_cast<uint32_t>(frameBufferLayouts.size());
		FBsamplerAllocInfo.pSetLayouts = frameBufferLayouts.data();

		m_DescriptorSets.resize(vkBackends->maxFramesInFlight);
		CHONPS_CORE_ASSERT(vkAllocateDescriptorSets(vkBackends->device, &FBsamplerAllocInfo, m_DescriptorSets.data()) == VK_SUCCESS, "Failed to allocate FrameBuffer descriptor sets!");

		for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
			UpdateFrameBufferImage(i);
	}

	void VulkanFrameBuffer::Draw(Shader* shader, uint32_t index)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		VkPipelineLayout pipelineLayout = static_cast<VulkanShader*>(shader)->getNativePipelineLayout();
		vkCmdBindDescriptorSets(vkBackends->commandBuffers[vkBackends->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, m_SetIndex, 1, &m_DescriptorSets[vkBackends->currentFrame], 0, nullptr);

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &m_VertexBuffer, offsets);
		vkCmdDraw(vkBackends->commandBuffers[vkBackends->currentFrame], 6, 1, 0, 0);
	}

	void VulkanFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		if (width != m_OffscreenPass.width || height != m_OffscreenPass.height)
		{
			m_OffscreenPass.width = width;
			m_OffscreenPass.height = height;
			m_FrameBufferSpecificationInfo.width = width;
			m_FrameBufferSpecificationInfo.height = height;

			vkDeviceWaitIdle(vkBackends->device);

			vkDestroyFramebuffer(vkBackends->device, m_OffscreenPass.framebuffer.framebuffer, nullptr);

			for (auto& colorAttachment : m_OffscreenPass.framebuffer.color)
			{
				vkDestroyImageView(vkBackends->device, colorAttachment.view, nullptr);
				vkDestroyImage(vkBackends->device, colorAttachment.image, nullptr);
				vmaFreeMemory(vmaAllocator, colorAttachment.memory);
			}

			for (auto& colorAttachment : m_OffscreenPass.framebuffer.msaaColor)
			{
				vkDestroyImageView(vkBackends->device, colorAttachment.view, nullptr);
				vkDestroyImage(vkBackends->device, colorAttachment.image, nullptr);
				vmaFreeMemory(vmaAllocator, colorAttachment.memory);
			}

			vkDestroyImageView(vkBackends->device, m_OffscreenPass.framebuffer.depth.view, nullptr);
			vkDestroyImage(vkBackends->device, m_OffscreenPass.framebuffer.depth.image, nullptr);
			vmaFreeMemory(vmaAllocator, m_OffscreenPass.framebuffer.depth.memory);

			CreateFrameBuffer(m_OffscreenPass.renderPass, &m_OffscreenPass.framebuffer, m_FrameBufferSpecificationInfo);
			Viewport(0, 0, width, height);

			for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
				UpdateFrameBufferImage(i);
		}
	}

	void VulkanFrameBuffer::Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		m_VPx = x;
		m_VPy = y;
		m_VPWidth = width < m_FrameBufferSpecificationInfo.width ? width : m_FrameBufferSpecificationInfo.width;
		m_VPHeight = height < m_FrameBufferSpecificationInfo.height ? height : m_FrameBufferSpecificationInfo.height;
	}

	void VulkanFrameBuffer::Bind()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_OffscreenPass.renderPass;
		renderPassInfo.framebuffer = m_OffscreenPass.framebuffer.framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent.width = m_OffscreenPass.width;
		renderPassInfo.renderArea.extent.height = m_OffscreenPass.height;

		for (auto& attachment : m_ColorAttachments)
			m_ClearValues[attachment.index].color = vkBackends->clearColor.color;

		if (m_DepthAttached)
			m_ClearValues[m_DepthAttachment.index].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(m_ClearValues.size());
		renderPassInfo.pClearValues = m_ClearValues.data();

		VkViewport viewport{};
		viewport.x = static_cast<float>(m_VPx);
		viewport.y = static_cast<float>(m_OffscreenPass.height - m_VPy);
		viewport.width = static_cast<float>(m_VPWidth);
		viewport.height = -static_cast<float>(m_VPHeight);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = m_OffscreenPass.width;
		scissor.extent.height = m_OffscreenPass.height;
		vkCmdSetScissor(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &scissor);

		vkCmdBeginRenderPass(vkBackends->commandBuffers[vkBackends->currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanFrameBuffer::Unbind()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		vkCmdEndRenderPass(vkBackends->commandBuffers[vkBackends->currentFrame]);
	}

	void VulkanFrameBuffer::Begin()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_OffscreenPass.renderPass;
		renderPassInfo.framebuffer = m_OffscreenPass.framebuffer.framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent.width = m_OffscreenPass.width;
		renderPassInfo.renderArea.extent.height = m_OffscreenPass.height;

		for (auto& attachment : m_ColorAttachments)
			m_ClearValues[attachment.index].color = vkBackends->clearColor.color;

		if (m_DepthAttached)
			m_ClearValues[m_DepthAttachment.index].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(m_ClearValues.size());
		renderPassInfo.pClearValues = m_ClearValues.data();

		VkViewport viewport{};
		viewport.x = static_cast<float>(m_VPx);
		viewport.y = static_cast<float>(m_OffscreenPass.height - m_VPy);
		viewport.width = static_cast<float>(m_VPWidth);
		viewport.height = -static_cast<float>(m_VPHeight);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = m_OffscreenPass.width;
		scissor.extent.height = m_OffscreenPass.height;
		vkCmdSetScissor(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &scissor);

		vkCmdBeginRenderPass(vkBackends->commandBuffers[vkBackends->currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanFrameBuffer::End()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		vkCmdEndRenderPass(vkBackends->commandBuffers[vkBackends->currentFrame]);
	}

	void VulkanFrameBuffer::BindTex(uint32_t slot)
	{

	}

	void VulkanFrameBuffer::Delete()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		vkBackends->frameBufferCountIDs.push(m_ID);

		vkDestroyFramebuffer(vkBackends->device, m_OffscreenPass.framebuffer.framebuffer, nullptr);

		for (auto& colorAttachment : m_OffscreenPass.framebuffer.color)
		{
			vkDestroyImageView(vkBackends->device, colorAttachment.view, nullptr);
			vkDestroyImage(vkBackends->device, colorAttachment.image, nullptr);
			vmaFreeMemory(vmaAllocator, colorAttachment.memory);
		}

		for (auto& colorAttachment : m_OffscreenPass.framebuffer.msaaColor)
		{
			vkDestroyImageView(vkBackends->device, colorAttachment.view, nullptr);
			vkDestroyImage(vkBackends->device, colorAttachment.image, nullptr);
			vmaFreeMemory(vmaAllocator, colorAttachment.memory);
		}

		if (m_FrameBufferSpecificationInfo.depthAttachment != nullptr)
		{
			vkDestroyImageView(vkBackends->device, m_OffscreenPass.framebuffer.depth.view, nullptr);
			vkDestroyImage(vkBackends->device, m_OffscreenPass.framebuffer.depth.image, nullptr);
			vmaFreeMemory(vmaAllocator, m_OffscreenPass.framebuffer.depth.memory);
		}

		vkDestroyBuffer(vkBackends->device, m_VertexBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, m_VertexBufferMemory);

		vkDestroySampler(vkBackends->device, m_OffscreenPass.sampler, nullptr);
		vkDestroyRenderPass(vkBackends->device, m_OffscreenPass.renderPass, nullptr);
	}
}

