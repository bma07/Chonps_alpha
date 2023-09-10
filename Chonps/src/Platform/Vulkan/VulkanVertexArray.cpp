#include "cepch.h"
#include "VulkanVertexArray.h"

#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"

namespace Chonps
{
	namespace vks
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
	}

	VulkanVertexArray::VulkanVertexArray()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		m_ID = vkBackends->vertexArrayBufferCountIDs.take_next();
	}

	void VulkanVertexArray::LinkBuffers(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, VertexLayoutLinkInfo* vertexLayouts)
	{
		CHONPS_CORE_ASSERT(vertexBuffer != nullptr && indexBuffer != nullptr, "Vertex Buffer or Index Buffer was nullptr!");

		VulkanVertexBuffer* vkVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);
		VulkanBufferData vertexBufferData = vkVertexBuffer->getBufferData();

		VulkanIndexBuffer* vkIndexBuffer = static_cast<VulkanIndexBuffer*>(indexBuffer);
		VulkanBufferData indexBufferData = vkIndexBuffer->getBufferData();

		VkDeviceSize bufferSize = vertexBufferData.size + indexBufferData.size;
		m_VertexArrayBufferSize = bufferSize;

		m_StateStatic = vertexBufferData.state == BufferState::Static && indexBufferData.state == BufferState::Static;

		// Method: we only create one large vertex array buffer that contains both the vertex and index buffer in the same memory allocation
		// If either buffers are not static, we keep use buffers separately
		if (m_StateStatic)
		{
			vks::createBuffer(bufferSize, m_VertexArrayBuffer, m_VertexArrayBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

			// Vertex Buffer
			vks::copyBuffer(vertexBufferData.staging, m_VertexArrayBuffer, vertexBufferData.size);
			m_VertexCount = vertexBuffer->GetCount();
			m_VertexBufferSize = vertexBufferData.size;

			// Index Buffer
			vks::copyBuffer(indexBufferData.staging, m_VertexArrayBuffer, indexBufferData.size, 0, vertexBufferData.size);
			m_IndexCount = indexBuffer->GetCount();
			m_IndexBufferSize = indexBufferData.size;
		}

		m_VertexBuffer = vertexBufferData.buffer;
		m_IndexBuffer = indexBufferData.buffer;

		// Vertex Layouts
		for (uint32_t i = 0; i < vertexLayouts->layoutCount; i++)
		{
			VertexLayout vertLayout = vertexLayouts->pLayouts[i];
			CHONPS_CORE_ASSERT(&vertLayout != nullptr, "Vertex Layout was nullptr!");
			VkVertexInputAttributeDescription attributeDescrption = vks::getAttributeDescriptions(vertLayout.layout, vks::getShaderDataTypeConvertVulkan(vertLayout.type, vertLayout.numComponents), vertLayout.stride, vertLayout.offset);
			m_AttributeDescriptions.push_back(attributeDescrption);
		}
	}

	void VulkanVertexArray::LinkBuffers(VertexArrayCreateInfo* createInfo)
	{
		CHONPS_CORE_ASSERT(createInfo->vertexBuffer != nullptr && createInfo->indexBuffer != nullptr, "Vertex Buffer or Index Buffer was nullptr!");

		VulkanVertexBuffer* vkVertexBuffer = static_cast<VulkanVertexBuffer*>(createInfo->vertexBuffer);
		VulkanBufferData vertexBufferData = vkVertexBuffer->getBufferData();

		VulkanIndexBuffer* vkIndexBuffer = static_cast<VulkanIndexBuffer*>(createInfo->indexBuffer);
		VulkanBufferData indexBufferData = vkIndexBuffer->getBufferData();

		VkDeviceSize bufferSize = vertexBufferData.size + indexBufferData.size;
		m_VertexArrayBufferSize = bufferSize;
		
		m_StateStatic = vertexBufferData.state == BufferState::Static && indexBufferData.state == BufferState::Static;

		// Method: we only create one large vertex array buffer that contains both the vertex and index buffer in the same memory allocation
		// If either buffers are not static, we keep use buffers separately
		if (m_StateStatic)
		{
			vks::createBuffer(bufferSize, m_VertexArrayBuffer, m_VertexArrayBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

			// Vertex Buffer
			vks::copyBuffer(vertexBufferData.staging, m_VertexArrayBuffer, vertexBufferData.size);
			m_VertexCount = createInfo->vertexBuffer->GetCount();
			m_VertexBufferSize = vertexBufferData.size;

			// Index Buffer
			vks::copyBuffer(indexBufferData.staging, m_VertexArrayBuffer, indexBufferData.size, 0, vertexBufferData.size);
			m_IndexCount = createInfo->indexBuffer->GetCount();
			m_IndexBufferSize = indexBufferData.size;
		}

		m_VertexBuffer = vertexBufferData.buffer;
		m_IndexBuffer = indexBufferData.buffer;

		// Vertex Layouts
		for (uint32_t i = 0; i < createInfo->vertexLayouts->layoutCount; i++)
		{
			VertexLayout vertLayout = createInfo->vertexLayouts->pLayouts[i];
			CHONPS_CORE_ASSERT(&vertLayout != nullptr, "Vertex Layout was nullptr!");
			VkVertexInputAttributeDescription attributeDescrption = vks::getAttributeDescriptions(vertLayout.layout, vks::getShaderDataTypeConvertVulkan(vertLayout.type, vertLayout.numComponents), vertLayout.stride, vertLayout.offset);
			m_AttributeDescriptions.push_back(attributeDescrption);
		}
	}

	void VulkanVertexArray::Bind()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		if (m_StateStatic)
		{
			if (m_VertexArrayBuffer == VK_NULL_HANDLE)
				return;

			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &m_VertexArrayBuffer, offsets);
			vkCmdBindIndexBuffer(vkBackends->commandBuffers[vkBackends->currentFrame], m_VertexArrayBuffer, m_VertexBufferSize, VK_INDEX_TYPE_UINT32);
		}
		else
		{
			if (m_VertexBuffer == VK_NULL_HANDLE || m_IndexBuffer == VK_NULL_HANDLE)
				return;

			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &m_VertexBuffer, offsets);
			vkCmdBindIndexBuffer(vkBackends->commandBuffers[vkBackends->currentFrame], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}
	
	void VulkanVertexArray::Unbind()
	{

	}
	
	void VulkanVertexArray::Delete()
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		vkDestroyBuffer(vkBackends->device, m_VertexArrayBuffer, nullptr);
		vmaFreeMemory(getVmaAllocator(), m_VertexArrayBufferMemory);

		vkBackends->vertexArrayBufferCountIDs.push(m_ID);
	}
}