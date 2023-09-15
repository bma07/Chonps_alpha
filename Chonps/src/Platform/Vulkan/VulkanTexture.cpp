#include "cepch.h"
#include "VulkanTexture.h"

#include <stb_image.h>

#include "Graphics/RendererBackends.h"
#include "VulkanRendererAPI.h"
#include "VulkanShader.h"

namespace Chonps
{
	VkFormat getVulkanFormat(TexType texType, TexFormat texFormat, int channels)
	{
		bool gammaCorrect = renderGetGammaCorrection();
		bool nonGammaTexture = texType != TexType::Specular && texType != TexType::Normal;
		VkFormat format = gammaCorrect == true ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
		if (texFormat != TexFormat::None && texFormat != TexFormat::RGB8 && texFormat != TexFormat::RGBA8 && gammaCorrect)
			CHONPS_CORE_WARN("WARNING: TEXTURE: Loaded texture with format that is not 'RGB8' or 'RGBA8' will not be gamma corrected!");

		switch (texFormat)
		{
			case Chonps::TexFormat::None:
			{
				gammaCorrect && nonGammaTexture ? format = VK_FORMAT_R8G8B8A8_SRGB : format = VK_FORMAT_R8G8B8A8_UNORM;
				break;
			}
			case Chonps::TexFormat::RGB8:
			{
				gammaCorrect && nonGammaTexture ? format = VK_FORMAT_R8G8B8_SRGB : format = VK_FORMAT_R8G8B8_UNORM;
				break;
			}
			case Chonps::TexFormat::RGB16:
			{
				gammaCorrect && nonGammaTexture ? format = VK_FORMAT_R16G16B16_SFLOAT : format = VK_FORMAT_R16G16B16_UNORM;
				break;
			}
			case Chonps::TexFormat::RGB32F:
			{
				format = VK_FORMAT_R32G32B32_SFLOAT;
				break;
			}
			case Chonps::TexFormat::RGB32I:
			{
				format = VK_FORMAT_R32G32B32_SINT;
				break;
			}
			case Chonps::TexFormat::RGB32UI:
			{
				format = VK_FORMAT_R32G32B32_UINT;
				break;
			}
			case Chonps::TexFormat::RGB64F:
			{
				format = VK_FORMAT_R64G64B64_SFLOAT;
				break;
			}
			case Chonps::TexFormat::RGB64I:
			{
				format = VK_FORMAT_R64G64B64_SINT;
				break;
			}
			case Chonps::TexFormat::RGB64UI:
			{
				format = VK_FORMAT_R64G64B64_UINT;
				break;
			}
			case Chonps::TexFormat::RGBA8:
			{
				gammaCorrect && nonGammaTexture ? format = VK_FORMAT_R8G8B8A8_SRGB : format = VK_FORMAT_R8G8B8A8_UNORM;
				break;
			}
			case Chonps::TexFormat::RGBA16:
			{
				gammaCorrect && nonGammaTexture ? format = VK_FORMAT_R16G16B16A16_SFLOAT : format = VK_FORMAT_R16G16B16A16_UNORM;
				break;
			}
			case Chonps::TexFormat::RGBA32F:
			{
				format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			}
			case Chonps::TexFormat::RGBA32I:
			{
				format = VK_FORMAT_R32G32B32A32_SINT;
				break;
			}
			case Chonps::TexFormat::RGBA32UI:
			{
				format = VK_FORMAT_R32G32B32A32_UINT;
				break;
			}
			case Chonps::TexFormat::RGBA64F:
			{
				format = VK_FORMAT_R64G64B64A64_SFLOAT;
				break;
			}
			case Chonps::TexFormat::RGBA64I:
			{
				format = VK_FORMAT_R64G64B64A64_SINT;
				break;
			}
			case Chonps::TexFormat::RGBA64UI:
			{
				format = VK_FORMAT_R64G64B64A64_UINT;
				break;
			}
		}

		return format;
	}

	VulkanTexture::VulkanTexture(const std::string& filepath , TexType texType, TexFilterPair texFilter, TexWrap texWrap)
		: Texture(filepath, texType, texFilter, texWrap), m_TexType(texType), m_TexFilter(texFilter), m_TexWrap(texWrap)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		m_ID = vkBackends->textureCountIDs.take_next();

		int channels;
		stbi_uc* texData = stbi_load(filepath.c_str(), &m_Width, &m_Height, &channels, STBI_rgb_alpha);
		VkDeviceSize imageSize = m_Width * m_Height * 4;

		if (!texData)
		{
			CHONPS_CORE_ERROR("ERROR: TEXTURE: Failed to load texture data: {0}", filepath);
			CHONPS_CORE_ASSERT(false, "Failed to load texture");
		}

		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferMemory;

		vks::createBuffer(imageSize, stagingBuffer, stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* bufferData;
		vmaMapMemory(vmaAllocator, stagingBufferMemory, &bufferData);
		memcpy(bufferData, texData, static_cast<size_t>(imageSize));
		vmaUnmapMemory(vmaAllocator, stagingBufferMemory);

		stbi_image_free(texData);

		VkFormat format = getVulkanFormat(texType, TexFormat::RGBA8, channels);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(m_Width);
		imageInfo.extent.height = static_cast<uint32_t>(m_Height);
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

		CHONPS_CORE_ASSERT(vkCreateImage(vkBackends->device, &imageInfo, nullptr, &m_TexData.textureImage) == VK_SUCCESS, "Failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vkBackends->device, m_TexData.textureImage, &memRequirements);

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		CHONPS_CORE_ASSERT(vmaAllocateMemory(vmaAllocator, &memRequirements, &allocInfo, &m_TexData.textureImageMemory, nullptr) == VK_SUCCESS, "Failed to allocate texture memory!");
		vmaBindImageMemory(vmaAllocator, m_TexData.textureImageMemory, m_TexData.textureImage);

		vks::transitionImageLayout(m_TexData.textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		vks::copyBufferToImage(stagingBuffer, m_TexData.textureImage, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));

		vks::transitionImageLayout(m_TexData.textureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, stagingBufferMemory);

		m_TexData.textureImageView = vks::createImageView(m_TexData.textureImage, format, VK_IMAGE_ASPECT_COLOR_BIT);

		VkFilter minFilter = VK_FILTER_NEAREST, magFilter = VK_FILTER_NEAREST;
		switch (texFilter.min)
		{
		case TexFilter::Linear: { minFilter = VK_FILTER_LINEAR; break; }
		case TexFilter::Nearest: { minFilter = VK_FILTER_NEAREST; break; }
		case TexFilter::Linear_Mipmap_Linear: { minFilter = VK_FILTER_LINEAR; break; }
		case TexFilter::Linear_Mipmap_Nearest: { minFilter = VK_FILTER_LINEAR; break; }
		case TexFilter::Nearest_Mipmap_Linear: { minFilter = VK_FILTER_NEAREST; break; }
		case TexFilter::Nearest_Mipmap_Nearest: { minFilter = VK_FILTER_NEAREST; break; }
		default:
		{
			CHONPS_CORE_ERROR("ERROR: TEXTURE: Given texture min filter not supported!");
		}
		}

		switch (texFilter.mag)
		{
		case TexFilter::Linear: { magFilter = VK_FILTER_LINEAR; break; }
		case TexFilter::Nearest: { magFilter = VK_FILTER_NEAREST; break; }
		default:
		{
			CHONPS_CORE_ERROR("ERROR: TEXTURE: Given texture mag filter not supported!");
		}
		}

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = magFilter;
		samplerInfo.minFilter = minFilter;

		VkSamplerAddressMode texAddressMode;
		if (texWrap == TexWrap::Repeat || texWrap == TexWrap::Default) texAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		else if (texWrap == TexWrap::ClampToEdge) texAddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		else if (texWrap == TexWrap::MirroredRepeat) texAddressMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		else CHONPS_CORE_ASSERT(false, "Format not supported!");

		samplerInfo.addressModeU = texAddressMode;
		samplerInfo.addressModeV = texAddressMode;
		samplerInfo.addressModeW = texAddressMode;

		samplerInfo.anisotropyEnable = vkBackends->deviceFeatures.samplerAnisotropy;
		samplerInfo.maxAnisotropy = vkBackends->deviceProperties.limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		CHONPS_CORE_ASSERT(vkCreateSampler(vkBackends->device, &samplerInfo, nullptr, &m_TexData.textureSampler) == VK_SUCCESS, "Failed to create texture sampler!");
	}

	VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, const void* data, TexType texType, TexFilterPair texFilter, TexWrap texWrap)
		: Texture(width, height, data, texType, texFilter, texWrap), m_Width(width), m_Height(height)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		m_ID = vkBackends->textureCountIDs.take_next();

		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferMemory;

		VkDeviceSize imageSize = width * height * 4;

		vks::createBuffer(imageSize, stagingBuffer, stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* bufferData;
		vmaMapMemory(vmaAllocator, stagingBufferMemory, &bufferData);
		memcpy(bufferData, data, static_cast<size_t>(imageSize));
		vmaUnmapMemory(vmaAllocator, stagingBufferMemory);

		VkFormat format = getVulkanFormat(texType, TexFormat::RGBA8, 4);

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

		CHONPS_CORE_ASSERT(vkCreateImage(vkBackends->device, &imageInfo, nullptr, &m_TexData.textureImage) == VK_SUCCESS, "Failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vkBackends->device, m_TexData.textureImage, &memRequirements);

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		vmaAllocateMemory(vmaAllocator, &memRequirements, &allocInfo, &m_TexData.textureImageMemory, nullptr);
		vmaBindImageMemory(vmaAllocator, m_TexData.textureImageMemory, m_TexData.textureImage);

		vks::transitionImageLayout(m_TexData.textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		vks::copyBufferToImage(stagingBuffer, m_TexData.textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		vks::transitionImageLayout(m_TexData.textureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, stagingBufferMemory);

		m_TexData.textureImageView = vks::createImageView(m_TexData.textureImage, format, VK_IMAGE_ASPECT_COLOR_BIT);

		VkFilter minFilter = VK_FILTER_NEAREST, magFilter = VK_FILTER_NEAREST;
		switch (texFilter.min)
		{
		case TexFilter::Linear: { minFilter = VK_FILTER_LINEAR; break; }
		case TexFilter::Nearest: { minFilter = VK_FILTER_NEAREST; break; }
		case TexFilter::Linear_Mipmap_Linear: { minFilter = VK_FILTER_LINEAR; break; }
		case TexFilter::Linear_Mipmap_Nearest: { minFilter = VK_FILTER_LINEAR; break; }
		case TexFilter::Nearest_Mipmap_Linear: { minFilter = VK_FILTER_NEAREST; break; }
		case TexFilter::Nearest_Mipmap_Nearest: { minFilter = VK_FILTER_NEAREST; break; }
		default:
		{
			CHONPS_CORE_ERROR("ERROR: TEXTURE: Given texture min filter not supported!");
		}
		}

		switch (texFilter.mag)
		{
		case TexFilter::Linear: { magFilter = VK_FILTER_LINEAR; break; }
		case TexFilter::Nearest: { magFilter = VK_FILTER_NEAREST; break; }
		default:
		{
			CHONPS_CORE_ERROR("ERROR: TEXTURE: Given texture mag filter not supported!");
		}
		}

		VkSamplerAddressMode texAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		if (texWrap == TexWrap::Repeat || texWrap == TexWrap::Default) texAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		else if (texWrap == TexWrap::ClampToEdge) texAddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		else if (texWrap == TexWrap::MirroredRepeat) texAddressMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		else CHONPS_CORE_ASSERT(false, "Format not supported!");

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = magFilter;
		samplerInfo.minFilter = minFilter;

		samplerInfo.addressModeU = texAddressMode;
		samplerInfo.addressModeV = texAddressMode;
		samplerInfo.addressModeW = texAddressMode;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(vkBackends->physicalDevice, &properties);
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

		CHONPS_CORE_ASSERT(vkCreateSampler(vkBackends->device, &samplerInfo, nullptr, &m_TexData.textureSampler) == VK_SUCCESS, "Failed to create texture sampler!");

		m_TexType = TexType::Diffuse;
		m_TexFilter = { TexFilter::Linear, TexFilter::Linear };
		m_TexWrap = TexWrap::Repeat;
	}

	void VulkanTexture::Delete()
	{
		if (!m_Deleted)
		{
			VulkanBackends* vkBackends = getVulkanBackends();

			vkDestroySampler(vkBackends->device, m_TexData.textureSampler, nullptr);
			vkDestroyImageView(vkBackends->device, m_TexData.textureImageView, nullptr);
			vkDestroyImage(vkBackends->device, m_TexData.textureImage, nullptr);
			vmaFreeMemory(getVmaAllocator(), m_TexData.textureImageMemory);

			vkBackends->textureImages.erase(m_ID);
			vkBackends->textureCountIDs.push(m_ID);
			m_Deleted = true;
		}
	}

	VulkanTextureLayout::VulkanTextureLayout(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex)
		: TextureLayout(pTextures, textureCount, setIndex), m_SetIndex(setIndex)
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		CHONPS_CORE_ASSERT(textureCount < vkBackends->maxTextureBindingSlots, "cannot have more textures than the max texture binding slot!");

		for (uint32_t i = 0; i < textureCount; i++)
		{
			TextureCreateInfo textureInfo = pTextures[i];

			CHONPS_CORE_ASSERT(textureInfo.slot < vkBackends->maxTextureBindingSlots, "cannot have texture slot that is above the max texture binding slot!");

			if (m_Textures.find(textureInfo.slot) == m_Textures.end())
			{
				m_Textures[textureInfo.slot] = std::move(textureInfo.texture);
			}
			else CHONPS_CORE_WARN("WARNING: TEXTURE_LAYOUT: Texture with slot {0} already exists! Ignoring second texture slot", textureInfo.slot);
		}

		// Create Descriptor Set for texture array to contain all textures
		std::vector<VkDescriptorSetLayout> samplerLayouts(vkBackends->maxFramesInFlight, vkBackends->textureDescriptorSetLayout);
		VkDescriptorSetAllocateInfo samplerAllocInfo{};
		samplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		samplerAllocInfo.descriptorPool = vkBackends->descriptorPool;
		samplerAllocInfo.descriptorSetCount = static_cast<uint32_t>(samplerLayouts.size());
		samplerAllocInfo.pSetLayouts = samplerLayouts.data();

		m_TextureDescriptorSet.resize(vkBackends->maxFramesInFlight);
		CHONPS_CORE_ASSERT(vkAllocateDescriptorSets(vkBackends->device, &samplerAllocInfo, m_TextureDescriptorSet.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");

		// Fill imageInfos with texture data
		m_ImageInfo = vkBackends->nullImageInfos;
		for (auto& texture : m_Textures)
		{
			VulkanTextureData* texData = static_cast<VulkanTexture*>(texture.second)->getNativeTextureData();
			m_ImageInfo[texture.first].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_ImageInfo[texture.first].imageView = texData->textureImageView;
			m_ImageInfo[texture.first].sampler = texData->textureSampler;
		}

		// Update texture data
		for (uint32_t i = 0; i < m_TextureDescriptorSet.size(); i++)
		{
			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_TextureDescriptorSet[i];
			descriptorWrites[0].dstBinding = vkBackends->textureDescriptorBindings->textureBinding;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			descriptorWrites[0].descriptorCount = vkBackends->maxTextureBindingSlots;
			descriptorWrites[0].pImageInfo = m_ImageInfo.data();

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_TextureDescriptorSet[i];
			descriptorWrites[1].dstBinding = vkBackends->textureDescriptorBindings->samplerBinding;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			descriptorWrites[1].descriptorCount = vkBackends->maxTextureBindingSlots;
			descriptorWrites[1].pImageInfo = m_ImageInfo.data();

			vkUpdateDescriptorSets(vkBackends->device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}
	}

	void VulkanTextureLayout::insert(Texture* texture, uint32_t slot)
	{
		if (m_Textures.find(slot) == m_Textures.end())
		{
			m_Textures[slot] = std::move(texture);
		}
		else CHONPS_CORE_WARN("WARNING: TEXTURE_LAYOUT: Texture with slot {0} already exists! Ignoring second texture slot", slot);
	}

	void VulkanTextureLayout::erase(uint32_t slot)
	{
		if (m_Textures.find(slot) == m_Textures.end())
			CHONPS_CORE_WARN("WARNING: TEXTURE_LAYOUT: No texture was found at slot {0}! texture cannot be removed because it does not exist!", slot);
		else
			m_Textures.erase(slot);
	}

	void VulkanTextureLayout::Bind(Shader* shader) const
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		VkPipelineLayout pipelineLayout = static_cast<VulkanShader*>(shader)->getNativePipelineLayout();
		vkCmdBindDescriptorSets(vkBackends->commandBuffers[vkBackends->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, m_SetIndex, 1, &m_TextureDescriptorSet[vkBackends->currentFrame], 0, nullptr);
	}

	void VulkanTextureLayout::Unbind() const
	{

	}

	void VulkanTextureLayout::Delete()
	{
		for (auto& texture : m_Textures)
			texture.second->Delete();
	}
}