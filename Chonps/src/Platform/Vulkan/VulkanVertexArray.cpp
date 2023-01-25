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
		m_VertexCount = VBO->GetCount();

		if (!m_FirstVertexInit)
		{
			VkDeviceSize bufferSize = getVulkanBackends()->vertexBufferSize;
			vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);
			vkSpec::copyBuffer(getVulkanBackends()->stagingVertexBuffer, m_VertexBuffer, bufferSize);
			m_FirstVertexInit = true;
		}
		else
		{
			VkDevice device = getVulkanBackends()->device;
			vkDestroyBuffer(device, m_VertexBuffer, nullptr);
			vkFreeMemory(device, m_VertexBufferMemory, nullptr);

			VkDeviceSize bufferSize = getVulkanBackends()->vertexBufferSize;
			vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);
			vkSpec::copyBuffer(getVulkanBackends()->stagingVertexBuffer, m_VertexBuffer, bufferSize);
		}

		VkVertexInputAttributeDescription2EXT attributeDescrption = vkSpec::getAttributeDescriptions(layout, getShaderDataTypeConvertVulkan(type, numComponents), stride, offset);
		m_AttributeDescriptions.push_back(attributeDescrption);
	}
	
	void VulkanVertexArray::LinkIndexBuffer(IndexBuffer* IBO)
	{
		IBO->Bind();
		m_IndexCount = IBO->GetCount();

		if (!m_FirstIndexInit)
		{
			VkDeviceSize bufferSize = getVulkanBackends()->indexBufferSize;
			vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);
			vkSpec::copyBuffer(getVulkanBackends()->stagingIndexBuffer, m_IndexBuffer, bufferSize);
			m_FirstIndexInit = true;
		}
		else
		{
			VkDevice device = getVulkanBackends()->device;
			vkDestroyBuffer(device, m_IndexBuffer, nullptr);
			vkFreeMemory(device, m_IndexBufferMemory, nullptr);

			VkDeviceSize bufferSize = getVulkanBackends()->indexBufferSize;
			vkSpec::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);
			vkSpec::copyBuffer(getVulkanBackends()->stagingIndexBuffer, m_IndexBuffer, bufferSize);
		}
	}
	
	void VulkanVertexArray::Bind() const
	{
		getVulkanBackends()->vertexBuffer = m_VertexBuffer;
		getVulkanBackends()->indexBuffer = m_IndexBuffer;
		getVulkanBackends()->attributeDescriptions = m_AttributeDescriptions;
	}
	
	void VulkanVertexArray::Unbind() const
	{
		getVulkanBackends()->vertexBuffer = VK_NULL_HANDLE;
		getVulkanBackends()->indexBuffer = VK_NULL_HANDLE;
		getVulkanBackends()->attributeDescriptions = std::vector<VkVertexInputAttributeDescription2EXT>();
	}
	
	void VulkanVertexArray::Delete()
	{
		VkDevice device = getVulkanBackends()->device;

		vkDestroyBuffer(device, m_VertexBuffer, nullptr);
		vkDestroyBuffer(device, m_IndexBuffer, nullptr);
		vkFreeMemory(device, m_VertexBufferMemory, nullptr);
		vkFreeMemory(device, m_IndexBufferMemory, nullptr);
	}
}