#include "cepch.h"
#include "VulkanTexture.h"

#include <stb_image.h>

#include "VulkanRendererAPI.h"

namespace Chonps
{
	VkFormat getVulkanFormat(TexType texType, TexFormat texFormat, int channels)
	{
		bool gammaCorrect = renderGetGammaCorrection();
		bool nonGammaTexture = texType != TexT::Specular && texType != TexT::Normal;
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

	VulkanTexture::VulkanTexture(const std::string& filepath, TexType texType, TexFormat texFormat, TexFilterPair texFilter, TexWrap texWrap)
		: Texture(filepath, texType, texFormat, texFilter, texWrap), m_TexType(texType), m_TexFormat(texFormat), m_TexFilter(texFilter), m_TexWrap(texWrap)
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		CHONPS_CORE_ASSERT(!vkBackends->textureCountIDs.empty(), "Reached max Texture count IDs!");

		m_ID = vkBackends->textureCountIDs.front();
		vkBackends->textureCountIDs.pop();

		int channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* texData = stbi_load(filepath.c_str(), &m_Width, &m_Height, &channels, STBI_rgb_alpha);
		stbi_set_flip_vertically_on_load(false);
		VkDeviceSize imageSize = m_Width * m_Height * 4;

		if (!texData)
		{
			CHONPS_CORE_ERROR("ERROR: TEXTURE: Failed to load texture data: {0}", filepath);
			CHONPS_CORE_ASSERT(false, "Failed to load texture");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		vkSpec::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* bufferData;
		vkMapMemory(vkBackends->device, stagingBufferMemory, 0, imageSize, 0, &bufferData);
		memcpy(bufferData, texData, static_cast<size_t>(imageSize));
		vkUnmapMemory(vkBackends->device, stagingBufferMemory);

		stbi_image_free(texData);

		VkFormat format = getVulkanFormat(texType, texFormat, channels);

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

		CHONPS_CORE_ASSERT(vkCreateImage(vkBackends->device, &imageInfo, nullptr, &m_TextureImage) == VK_SUCCESS, "Failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vkBackends->device, m_TextureImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = vkSpec::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		CHONPS_CORE_ASSERT(vkAllocateMemory(vkBackends->device, &allocInfo, nullptr, &m_TextureImageMemory) == VK_SUCCESS, "Failed to allocate image memory!");

		vkBindImageMemory(vkBackends->device, m_TextureImage, m_TextureImageMemory, 0);

		vkSpec::transitionImageLayout(m_TextureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		vkSpec::copyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));

		vkSpec::transitionImageLayout(m_TextureImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
		vkFreeMemory(vkBackends->device, stagingBufferMemory, nullptr);

		m_TextureImageView = vkSpec::createImageView(m_TextureImage, format, VK_IMAGE_ASPECT_COLOR_BIT);

		VkFilter minFilter = VK_FILTER_NEAREST, magFilter = VK_FILTER_NEAREST;
		switch (texFilter.min)
		{
			case TexFilter::Linear:
			{
				minFilter = VK_FILTER_LINEAR;
				break;
			}
			case TexFilter::Nearest:
			{
				minFilter = VK_FILTER_NEAREST;
				break;
			}
			case TexFilter::Linear_Mipmap_Linear:
			{
				minFilter = VK_FILTER_LINEAR;
				break;
			}
			case TexFilter::Linear_Mipmap_Nearest:
			{
				minFilter = VK_FILTER_LINEAR;
				break;
			}
			case TexFilter::Nearest_Mipmap_Linear:
			{
				minFilter = VK_FILTER_NEAREST;
				break;
			}
			case TexFilter::Nearest_Mipmap_Nearest:
			{
				minFilter = VK_FILTER_NEAREST;
				break;
			}
			default:
			{
				CHONPS_CORE_ERROR("ERROR: TEXTURE: Given texture min filter not supported!");
			}
		}

		switch (texFilter.mag)
		{
			case TexFilter::Linear:
			{
				magFilter = VK_FILTER_LINEAR;
				break;
			}
			case TexFilter::Nearest:
			{
				magFilter = VK_FILTER_NEAREST;
				break;
			}
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
		else if (texWrap == TexWrap::ClampEdge) texAddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		else if (texWrap == TexWrap::MirroredRepeat) texAddressMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		else CHONPS_CORE_ASSERT(false, "Format not supported!");

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

		CHONPS_CORE_ASSERT(vkCreateSampler(vkBackends->device, &samplerInfo, nullptr, &m_TextureSampler) == VK_SUCCESS, "Failed to create texture sampler!");

		VulkanTextureData vulkanTexData{};
		vulkanTexData.textureImage = m_TextureImage;
		vulkanTexData.textureImageMemory = m_TextureImageMemory;
		vulkanTexData.textureImageView = m_TextureImageView;
		vulkanTexData.textureSampler = m_TextureSampler;

		vkBackends->textureImages[m_ID] = vulkanTexData;
		
		for (auto& queue : vkBackends->texturesQueue)
			queue.push(m_ID);
	}

	VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, void* data, uint32_t size)
		: Texture(width, height, data, size), m_Width(width), m_Height(height)
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VkDeviceSize imageSize = width * height * 4;

		vkSpec::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* bufferData;
		vkMapMemory(vkBackends->device, stagingBufferMemory, 0, imageSize, 0, &bufferData);
		memcpy(bufferData, data, static_cast<size_t>(imageSize));
		vkUnmapMemory(vkBackends->device, stagingBufferMemory);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = renderGetGammaCorrection() == true ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional

		CHONPS_CORE_ASSERT(vkCreateImage(vkBackends->device, &imageInfo, nullptr, &m_TextureImage) == VK_SUCCESS, "Failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vkBackends->device, m_TextureImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = vkSpec::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		CHONPS_CORE_ASSERT(vkAllocateMemory(vkBackends->device, &allocInfo, nullptr, &m_TextureImageMemory) == VK_SUCCESS, "Failed to allocate image memory!");

		vkBindImageMemory(vkBackends->device, m_TextureImage, m_TextureImageMemory, 0);

		vkSpec::transitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		vkSpec::copyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		vkSpec::transitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
		vkFreeMemory(vkBackends->device, stagingBufferMemory, nullptr);

		m_TextureImageView = vkSpec::createImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

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

		CHONPS_CORE_ASSERT(vkCreateSampler(vkBackends->device, &samplerInfo, nullptr, &m_TextureSampler) == VK_SUCCESS, "Failed to create texture sampler!");

		VulkanTextureData texData{};
		texData.textureImage = m_TextureImage;
		texData.textureImageMemory = m_TextureImageMemory;
		texData.textureImageView = m_TextureImageView;
		texData.textureSampler = m_TextureSampler;

		vkBackends->textureImages[m_ID] = texData;
		
		for (auto& queue : vkBackends->texturesQueue)
			queue.push(m_ID);

		m_TexType = TexType::Diffuse;
		m_TexFilter = { TexFilter::Linear, TexFilter::Linear };
		m_TexWrap = TexWrap::Repeat;
		m_TexFormat = TexFormat::RGBA8;
	}

	void VulkanTexture::Bind(uint32_t unit) const
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		if (vkBackends->allowClearBindedTextures) vkBackends->currentBindedTextureImages.clear();
		vkBackends->currentBindedTextureImages.insert(m_ID);
		vkBackends->textureImages[m_ID].unit = unit;

		vkBackends->textureIndexConstant.texIndex[unit] = m_ID;

		vkBackends->allowClearBindedTextures = false;
	}

	void VulkanTexture::Bind() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		if (vkBackends->allowClearBindedTextures) vkBackends->currentBindedTextureImages.clear();
		vkBackends->currentBindedTextureImages.insert(m_ID);
		vkBackends->textureImages[m_ID].unit = m_Unit;

		vkBackends->textureIndexConstant.texIndex[m_Unit] = m_ID;

		vkBackends->allowClearBindedTextures = false;
	}

	void VulkanTexture::Unbind() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		vkBackends->currentBindedTextureImages.erase(m_ID);
	}

	void VulkanTexture::Delete()
	{
		if (!m_Deleted)
		{
			VulkanBackends* vkBackends = getVulkanBackends();

			vkDestroySampler(vkBackends->device, m_TextureSampler, nullptr);
			vkDestroyImageView(vkBackends->device, m_TextureImageView, nullptr);
			vkDestroyImage(vkBackends->device, m_TextureImage, nullptr);
			vkFreeMemory(vkBackends->device, m_TextureImageMemory, nullptr);

			vkBackends->textureImages.erase(m_ID);
			vkBackends->textureCountIDs.push(m_ID);
			m_Deleted = true;
		}
	}

	void VulkanTexture::TexUnit(Shader* shader, const char* uniform, uint32_t unit)
	{
		VulkanBackends* vkbackends = getVulkanBackends();
		shader->Bind();
		if (unit < vkbackends->maxTextures)
		{
			m_Unit = unit;
			vkbackends->textureImages[m_ID].unit = unit;
		}
		else CHONPS_CORE_WARN("WARNING: TEXTURE: Attempted to bind texture to unit {0}! Cannot have texture unit heigher than max texture unit slots which is {1}!", unit, vkbackends->maxTextures);
	}

	void VulkanTexture::TexUnit(uint32_t unit)
	{
		m_Unit = unit;
		getVulkanBackends()->textureImages[m_ID].unit = unit;
	}

	namespace vkSpec
	{
		void vkImplTextureBinding(uint32_t textureBinding, uint32_t samplerBinding)
		{
			CHONPS_CORE_ASSERT(textureBinding != samplerBinding, "textureBinding cannot be the same value as samplerBinding!");

			getVulkanBackends()->textureBinding = textureBinding;
			getVulkanBackends()->samplerBinding = samplerBinding;
		}
	}
}