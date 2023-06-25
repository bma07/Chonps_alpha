#include "cepch.h"
#include "VulkanVertexArray.h"

#include "VulkanVertexBuffer.h"

namespace Chonps
{
	VkFormat getShaderDataTypeConvertVulkan(ShaderDataType type, uint32_t numComponents)
	{
		switch (type)
		{
			case Chonps::ShaderDataType::None: 
			{
				return VK_FORMAT_UNDEFINED;
			}
			case Chonps::ShaderDataType::Float: 
			{
				if (numComponents == 1) return VK_FORMAT_R32_SFLOAT;
				else if (numComponents == 2) return VK_FORMAT_R32G32_SFLOAT;
				else if (numComponents == 3) return VK_FORMAT_R32G32B32_SFLOAT;
				else if (numComponents == 4) return VK_FORMAT_R32G32B32A32_SFLOAT;
				else CHONPS_CORE_ERROR("ERROR: VULKAN: Could not find the corresponding shader data type given the numComponents!");
				return VK_FORMAT_UNDEFINED;
			}
			case Chonps::ShaderDataType::Mat2: 
			{
				return VK_FORMAT_R32G32_SFLOAT;
			}
			case Chonps::ShaderDataType::Mat3: 
			{
				return VK_FORMAT_R32G32B32_SFLOAT;
			}
			case Chonps::ShaderDataType::Mat4: 
			{
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			case Chonps::ShaderDataType::Int: 
			{
				if (numComponents == 1) return VK_FORMAT_R32_SINT;
				else if (numComponents == 2) return VK_FORMAT_R32G32_SINT;
				else if (numComponents == 3) return VK_FORMAT_R32G32B32_SINT;
				else if (numComponents == 4) return VK_FORMAT_R32G32B32A32_SINT;
				else CHONPS_CORE_ERROR("ERROR: VULKAN: Could not find the corresponding shader data type given the numComponents!");
				return VK_FORMAT_UNDEFINED;
			}
			case Chonps::ShaderDataType::Bool: 
			{
				return VK_FORMAT_R8_UINT;
			}
		}

		return VK_FORMAT_UNDEFINED;
	}

	VulkanVertexArray::VulkanVertexArray()
	{

	}

	void VulkanVertexArray::LinkVertexBuffer(VertexBuffer* VBO, uint32_t layout, uint32_t numComponents, ShaderDataType type, uint32_t stride, void* offset)
	{
		VBO->Bind();

		if (m_VertexBuffer != VBO)
		{
			VkDeviceSize bufferSize = getVulkanBackends()->vertexBufferSize;
			vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VkVertexBuffer, m_VertexBufferMemory);
			vkSpec::copyBuffer(getVulkanBackends()->stagingVertexBuffer, m_VkVertexBuffer, bufferSize);
		}
		
		m_VertexCount = VBO->GetCount();
		m_VertexBuffer = VBO;

		VkVertexInputAttributeDescription2EXT attributeDescrption = vkSpec::getAttributeDescriptions(layout, getShaderDataTypeConvertVulkan(type, numComponents), stride, offset);
		m_AttributeDescriptions.push_back(attributeDescrption);
	}
	
	void VulkanVertexArray::LinkIndexBuffer(IndexBuffer* IBO)
	{
		IBO->Bind();

		if (m_IndexBuffer != IBO)
		{
			VkDeviceSize bufferSize = getVulkanBackends()->indexBufferSize;
			vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VkIndexBuffer, m_IndexBufferMemory);
			vkSpec::copyBuffer(getVulkanBackends()->stagingIndexBuffer, m_VkIndexBuffer, bufferSize);
		}
		
		m_IndexCount = IBO->GetCount();
		m_IndexBuffer = IBO;
	}
	
	void VulkanVertexArray::Bind() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		vkBackends->vertexBuffer = m_VkVertexBuffer;
		vkBackends->indexBuffer = m_VkIndexBuffer;
		vkBackends->attributeDescriptions = m_AttributeDescriptions;
	}
	
	void VulkanVertexArray::Unbind() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		vkBackends->vertexBuffer = VK_NULL_HANDLE;
		vkBackends->indexBuffer = VK_NULL_HANDLE;
		vkBackends->attributeDescriptions.clear();
	}
	
	void VulkanVertexArray::Delete()
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		vkDestroyBuffer(vkBackends->device, m_VkVertexBuffer, nullptr);
		vkDestroyBuffer(vkBackends->device, m_VkIndexBuffer, nullptr);
		vkFreeMemory(vkBackends->device, m_VertexBufferMemory, nullptr);
		vkFreeMemory(vkBackends->device, m_IndexBufferMemory, nullptr);
	}
}