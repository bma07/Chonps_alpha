#include "cepch.h"
#include "VulkanCubemap.h"

#include <stb_image.h>

#include "VulkanShader.h"

namespace Chonps
{
	float vkImplCubemapVertices[] =
	{
		//   Coordinates
		-1.0f, -1.0f,  1.0f, // 0       7--------6
		 1.0f, -1.0f,  1.0f, // 1      /|       /|
		 1.0f, -1.0f, -1.0f, // 2     4--------5 |
		-1.0f, -1.0f, -1.0f, // 3     | |      | |
		-1.0f,  1.0f,  1.0f, // 4     | 3------|-2
		 1.0f,  1.0f,  1.0f, // 5     |/       |/ 
		 1.0f,  1.0f, -1.0f, // 6     0--------1  
		-1.0f,  1.0f, -1.0f  // 7	  
	};

	uint32_t vkImplCubemapIndices[] =
	{
		// Right
		6, 2, 1,
		6, 1, 5,
		// Left
		4, 0, 3,
		4, 3, 7,
		// Top
		5, 4, 7,
		5, 7, 6,
		// Bottom
		3, 0, 1,
		3, 1, 2,
		// Front
		5, 1, 0,
		5, 0, 4,
		// Back
		7, 3, 2,
		7, 2, 6
	};

	VulkanCubemap::VulkanCubemap(CubemapCreateInfo cubemapInfo, uint32_t setIndex)
		: Cubemap(cubemapInfo, setIndex), m_SetIndex(setIndex)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		const std::string cubeMapFaces[6] = { cubemapInfo.posx, cubemapInfo.negx, cubemapInfo.posy, cubemapInfo.negy, cubemapInfo.posz, cubemapInfo.negz };
		
		std::vector<stbi_uc> texData;
		stbi_uc* texFaces[6];

		VkDeviceSize layerSize = 0;

		int channels, width, height;
		stbi_set_flip_vertically_on_load(false);
		for (uint32_t i = 0; i < 6; i++)
		{
			texFaces[i] = stbi_load(cubeMapFaces[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);

			if (!texFaces[i])
			{
				CHONPS_CORE_LOG_ERROR(Vulkan:Cubemap:Texture, "Failed to load texture data: {0}", cubeMapFaces[i]);
				CHONPS_CORE_ASSERT(false, "Failed to load texture");
			}

			layerSize = width * height * 4;
			texData.insert(texData.end(), texFaces[i], texFaces[i] + layerSize);
		}

		VkDeviceSize imageSize = layerSize * 6;

		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferMemory;

		vks::createBuffer(imageSize, stagingBuffer, stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* bufferData;
		vmaMapMemory(vmaAllocator, stagingBufferMemory, &bufferData);
		memcpy(bufferData, texData.data(), static_cast<size_t>(imageSize));
		vmaUnmapMemory(vmaAllocator, stagingBufferMemory);

		for (uint32_t i = 0; i < 6; i++)
			stbi_image_free(texFaces[i]);

		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 6; // cubemap has 6 sides/images
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

		CHONPS_CORE_ASSERT(vkCreateImage(vkBackends->device, &imageInfo, nullptr, &m_CubemapImage) == VK_SUCCESS, "Failed to create Cubemap image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vkBackends->device, m_CubemapImage, &memRequirements);

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		CHONPS_CORE_ASSERT(vmaAllocateMemory(vmaAllocator, &memRequirements, &allocInfo, &m_CubemapImageMemory, nullptr) == VK_SUCCESS, "Failed to Allocate cubemap memory!");
		vmaBindImageMemory(vmaAllocator, m_CubemapImageMemory, m_CubemapImage);

		vks::transitionImageLayout(m_CubemapImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6);
		vks::copyBufferToImage(stagingBuffer, m_CubemapImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 6);

		vks::transitionImageLayout(m_CubemapImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 6);

		vkDestroyBuffer(vkBackends->device, stagingBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, stagingBufferMemory);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_CubemapImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 6;

		CHONPS_CORE_ASSERT(vkCreateImageView(vkBackends->device, &viewInfo, nullptr, &m_CubemapImageView) == VK_SUCCESS, "Failed to create Cubemap texture image view!");

		// Sampler
		VkSamplerCreateInfo sampler{};
		sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler.magFilter = VK_FILTER_LINEAR;
		sampler.minFilter = VK_FILTER_LINEAR;
		sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler.mipLodBias = 0.0f;
		sampler.minLod = 0.0f;
		sampler.maxLod = 0.0f;
		sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		sampler.maxAnisotropy = 1.0f;
		if (vkBackends->deviceFeatures.samplerAnisotropy)
		{
			sampler.maxAnisotropy = vkBackends->deviceProperties.limits.maxSamplerAnisotropy;
			sampler.anisotropyEnable = VK_TRUE;
		}
		CHONPS_CORE_ASSERT(vkCreateSampler(vkBackends->device, &sampler, nullptr, &m_CubemapSampler) == VK_SUCCESS, "Failed to create Cubemap image sampler!");

		// Allocate Descriptor Sets
		std::vector<VkDescriptorSetLayout> cubemapLayouts(vkBackends->maxFramesInFlight, vkBackends->cubemapDescriptorSetLayout);
		VkDescriptorSetAllocateInfo cubemapSamplerAllocInfo{};
		cubemapSamplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		cubemapSamplerAllocInfo.descriptorPool = vkBackends->descriptorPool;
		cubemapSamplerAllocInfo.descriptorSetCount = static_cast<uint32_t>(cubemapLayouts.size());
		cubemapSamplerAllocInfo.pSetLayouts = cubemapLayouts.data();

		m_DescriptorSets.resize(vkBackends->maxFramesInFlight);
		CHONPS_CORE_ASSERT(vkAllocateDescriptorSets(vkBackends->device, &cubemapSamplerAllocInfo, m_DescriptorSets.data()) == VK_SUCCESS, "Failed to allocate Cubemap descriptor sets!");

		// Update Descriptor Sets
		for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
			UpdateDescriptorSets(i);

		// Vertex Buffer
		VkDeviceSize bufferSize = sizeof(vkImplCubemapVertices);

		VkBuffer vertexStagingBuffer;
		VmaAllocation vertexStagingBufferMemory;

		vks::createBuffer(bufferSize, vertexStagingBuffer, vertexStagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* vertexData;
		vmaMapMemory(vmaAllocator, vertexStagingBufferMemory, &vertexData);
		std::memcpy(vertexData, vkImplCubemapVertices, (size_t)bufferSize);
		vmaUnmapMemory(vmaAllocator, vertexStagingBufferMemory);

		vks::createBuffer(bufferSize, m_VertexBuffer, m_VertexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		vks::copyBuffer(vertexStagingBuffer, m_VertexBuffer, bufferSize);

		vkDestroyBuffer(vkBackends->device, vertexStagingBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, vertexStagingBufferMemory);

		// Index Buffer
		bufferSize = sizeof(vkImplCubemapIndices);

		VkBuffer indexStagingBuffer;
		VmaAllocation indexStagingBufferMemory;

		vks::createBuffer(bufferSize, indexStagingBuffer, indexStagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* indexData;
		vmaMapMemory(vmaAllocator, indexStagingBufferMemory, &indexData);
		std::memcpy(indexData, vkImplCubemapIndices, (size_t)bufferSize);
		vmaUnmapMemory(vmaAllocator, indexStagingBufferMemory);

		vks::createBuffer(bufferSize, m_IndexBuffer, m_IndexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		vks::copyBuffer(indexStagingBuffer, m_IndexBuffer, bufferSize);

		vkDestroyBuffer(vkBackends->device, indexStagingBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, indexStagingBufferMemory);
	}

	void VulkanCubemap::Bind(Shader* shader)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		VkPipelineLayout pipelineLayout = static_cast<VulkanShader*>(shader)->getNativePipelineLayout();
		vkCmdBindDescriptorSets(vkBackends->commandBuffers[vkBackends->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, m_SetIndex, 1, &m_DescriptorSets[vkBackends->currentFrame], 0, nullptr);

		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &m_VertexBuffer, offsets);
		vkCmdBindIndexBuffer(vkBackends->commandBuffers[vkBackends->currentFrame], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(vkBackends->commandBuffers[vkBackends->currentFrame], 36, 1, 0, 0, 0);
	}

	void VulkanCubemap::Delete()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		vkDestroySampler(vkBackends->device, m_CubemapSampler, nullptr);
		vkDestroyImageView(vkBackends->device, m_CubemapImageView, nullptr);
		vkDestroyImage(vkBackends->device, m_CubemapImage, nullptr);
		vmaFreeMemory(vmaAllocator, m_CubemapImageMemory);

		vkDestroyBuffer(vkBackends->device, m_VertexBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, m_VertexBufferMemory);
		vkDestroyBuffer(vkBackends->device, m_IndexBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, m_IndexBufferMemory);
	}

	void VulkanCubemap::UpdateDescriptorSets(uint32_t currentImage)
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		VkDescriptorImageInfo imageInfo;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_CubemapImageView;
		imageInfo.sampler = m_CubemapSampler;

		VkWriteDescriptorSet  descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_DescriptorSets[currentImage];
		descriptorWrite.dstBinding = vkBackends->textureDescriptorBindings->cubemapBinding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(vkBackends->device, 1, &descriptorWrite, 0, nullptr);
	}
}