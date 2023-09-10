#include "cepch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanRendererAPI.h"

namespace Chonps
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t binding, uint32_t size, BufferType bufferType)
		: UniformBuffer(binding, size, bufferType), m_Binding(binding)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		m_ID = vkBackends->uniformBufferCountIDs.take_next();

		VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		switch (bufferType)
		{
			case Chonps::BufferType::None:
			{
				CHONPS_CORE_ERROR("ERROR: UNIFORM_BUFFER: No bufferType was chosen!");
				break;
			}
			case Chonps::BufferType::Uniform:
			{
				descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			}
			case Chonps::BufferType::Storage:
			{
				descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			}
		}

		
		VkDeviceSize bufferSize = bufferType == BufferType::Storage ? vkBackends->maxObjects * size : size;
		m_BufferData.bufferSize = bufferSize;
		
		// Create Uniform Buffers
		m_BufferData.buffers.resize(vkBackends->maxFramesInFlight);
		m_BufferData.buffersMemory.resize(vkBackends->maxFramesInFlight);
		m_BufferData.buffersMapped.resize(vkBackends->maxFramesInFlight);

		for (uint32_t i = 0; i < vkBackends->maxFramesInFlight; i++)
		{
			vks::createBuffer(bufferSize, m_BufferData.buffers[i], m_BufferData.buffersMemory[i], descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
			vmaMapMemory(vmaAllocator, m_BufferData.buffersMemory[i], &m_BufferData.buffersMapped[i]);
		}

		vkBackends->uniformBuffersData[m_ID] = &m_BufferData;
	}

	void VulkanUniformBuffer::Bind(const void* data, uint32_t size, uint32_t offset)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		if (data == nullptr || size == 0)
			return;

		void* objectData = m_BufferData.buffersMapped[vkBackends->currentFrame];
		objectData = static_cast<void*>(static_cast<char*>(objectData) + offset);

		memcpy(objectData, static_cast<const char*>(data), size);
	}

	void VulkanUniformBuffer::Delete()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();
		for (size_t i = 0; i < getVulkanBackends()->maxFramesInFlight; i++)
		{
			vkDestroyBuffer(vkBackends->device, m_BufferData.buffers[i], nullptr);
			vmaUnmapMemory(vmaAllocator, m_BufferData.buffersMemory[i]);
			vmaFreeMemory(vmaAllocator, m_BufferData.buffersMemory[i]);
		}

		vkBackends->uniformBufferCountIDs.push(m_ID);
	}

	namespace vks
	{
		BufferLayout vkImplCreateBufferLayout(BufferBindingCreateLayoutsInfo* bufferBindings, uint32_t setIndex)
		{
			VulkanBackends* vkBackends = getVulkanBackends();
			
			// Create Descriptor Layout
			std::vector<VkDescriptorSetLayoutBinding> uboLayoutBinding; uboLayoutBinding.resize(bufferBindings->bindingCount);

			for (int i = 0; i < uboLayoutBinding.size(); i++)
			{
				BufferBindingInfo buffer = bufferBindings->pBufferBindings[i];

				VkDescriptorType descriptorType;
				if (buffer.bufferType == BufferType::Uniform)
					descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				else if (buffer.bufferType == BufferType::Storage)
					descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				else
					descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

				VkShaderStageFlagBits shaderStage{};
				if (buffer.shaderStage == ShaderStage::Vertex)
					shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
				else if (buffer.shaderStage == ShaderStage::Fragment)
					shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
				else if (buffer.shaderStage == ShaderStage::All)
					shaderStage = VK_SHADER_STAGE_ALL;

				uboLayoutBinding[i].binding = buffer.binding;
				uboLayoutBinding[i].descriptorType = descriptorType;
				uboLayoutBinding[i].descriptorCount = 1;
				uboLayoutBinding[i].stageFlags = shaderStage;
				uboLayoutBinding[i].pImmutableSamplers = nullptr; // Optional
			}
			VkDescriptorSetLayoutCreateInfo uboLayoutInfo{};
			uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			uboLayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
			uboLayoutInfo.bindingCount = static_cast<uint32_t>(uboLayoutBinding.size());
			uboLayoutInfo.pBindings = uboLayoutBinding.data();

			VkDescriptorSetLayout descriptorSetLayout{};

			CHONPS_CORE_ASSERT(vkCreateDescriptorSetLayout(vkBackends->device, &uboLayoutInfo, nullptr, &descriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");

			// Create Descriptor Sets
			std::vector<VkDescriptorSetLayout> uboLayouts(vkBackends->maxFramesInFlight, descriptorSetLayout);
			VkDescriptorSetAllocateInfo uboAllocInfo{};
			uboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			uboAllocInfo.descriptorPool = vkBackends->descriptorPool;
			uboAllocInfo.descriptorSetCount = static_cast<uint32_t>(uboLayouts.size());
			uboAllocInfo.pSetLayouts = uboLayouts.data();

			std::vector<VkDescriptorSet> descriptorSets;
			descriptorSets.resize(vkBackends->maxFramesInFlight);
			CHONPS_CORE_ASSERT(vkAllocateDescriptorSets(vkBackends->device, &uboAllocInfo, descriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");

			vkBackends->descriptorSetLayouts.push_back(descriptorSetLayout);
			vkBackends->descriptorSetLayoutIndex.push_back(std::pair<uint32_t, VkDescriptorSetLayout>(setIndex, descriptorSetLayout));

			for (uint32_t i = 0; i < bufferBindings->bindingCount; i++)
				vkBackends->bufferDescriptorSets[bufferBindings->pBufferBindings[i].uniformBuffer->id()] = descriptorSets;

			for (size_t i = 0; i < descriptorSets.size(); i++)
			{
				for (uint32_t j = 0; j < bufferBindings->bindingCount; j++)
				{
					BufferBindingInfo buffer = bufferBindings->pBufferBindings[j];
					VulkanUniformBufferData* bufferData = vkBackends->uniformBuffersData[buffer.uniformBuffer->id()];

					VkDescriptorBufferInfo bufferInfo{};
					bufferInfo.buffer = bufferData->buffers[i];
					bufferInfo.offset = 0;
					bufferInfo.range = bufferData->bufferSize;

					VkDescriptorType descriptorType;
					if (buffer.bufferType == BufferType::Uniform)
						descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					else if (buffer.bufferType == BufferType::Storage)
						descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					else
						descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

					bufferData->bufferType = descriptorType;

					VkWriteDescriptorSet descriptorWrites{};
					descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites.dstSet = descriptorSets[i];
					descriptorWrites.dstBinding = buffer.binding;
					descriptorWrites.dstArrayElement = 0;
					descriptorWrites.descriptorType = descriptorType;
					descriptorWrites.descriptorCount = 1;
					descriptorWrites.pBufferInfo = &bufferInfo;

					vkUpdateDescriptorSets(vkBackends->device, 1, &descriptorWrites, 0, nullptr);
				}
			}

			BufferLayout layout{};
			layout.layoutData = descriptorSetLayout;
			layout.setIndex = setIndex;

			return layout;
		}

		PushConstantRange vkImplCreatePushConstantRange(uint32_t size, uint32_t offset, ShaderStage shaderStage)
		{
			VkShaderStageFlags shaderStageFlags = VK_SHADER_STAGE_ALL;
			if (shaderStage == ShaderStage::Vertex)
				shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			else if (shaderStage == ShaderStage::Fragment)
				shaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			else if (shaderStage == ShaderStage::All)
				shaderStageFlags = VK_SHADER_STAGE_ALL;

			VkPushConstantRange pushConstant{};
			pushConstant.size = size;
			pushConstant.offset = offset;
			pushConstant.stageFlags = shaderStageFlags;

			PushConstantRange pc{};
			pc.pcData = &pushConstant;
			pc.size = size;
			pc.offset = offset;
			pc.shaderStage = shaderStage;

			return pc;
		}
	}
}