#include "cepch.h"
#include "VulkanIndexBuffer.h"

namespace Chonps
{
	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t size, BufferState bufferState)
		: IndexBuffer(indices, size), m_Count(size / sizeof(uint32_t)), m_Indices(indices), m_Size(size)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();
		m_ID = vkBackends->vertexBufferCountIDs.take_next();

		VkDeviceSize bufferSize = size;
		m_IndexBufferSize = bufferSize;

		// If buffer is set to dynamic, we skip the staging buffer part and instead use the buffer on CPU memory
		m_UseStaging = bufferState != BufferState::Dynamic && bufferState != BufferState::DynamicResize;

		VmaMemoryUsage bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		if (m_UseStaging) bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		else bufferMemoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

		if (m_UseStaging) // create the staging buffer to copy data
		{
			vks::createBuffer(bufferSize, m_StagingBuffer, m_StagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

			if (indices != nullptr)
			{
				void* data;
				vmaMapMemory(vmaAllocator, m_StagingBufferMemory, &data);
				std::memcpy(data, indices, bufferSize);
				vmaUnmapMemory(vmaAllocator, m_StagingBufferMemory);
			}
		}

		// Create the main buffer used for binding
		vks::createBuffer(bufferSize, m_IndexBuffer, m_IndexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferMemoryUsage);

		// Copy the staging buffer to the main buffer if using staging
		if (m_UseStaging) vks::copyBuffer(m_StagingBuffer, m_IndexBuffer, bufferSize);
		else
		{
			if (indices != nullptr)
			{
				void* data;
				vmaMapMemory(vmaAllocator, m_IndexBufferMemory, &data);
				std::memcpy(data, indices, bufferSize);
				vmaUnmapMemory(vmaAllocator, m_IndexBufferMemory);
			}
		}

		m_BufferData.buffer = m_IndexBuffer;
		m_BufferData.staging = m_StagingBuffer;
		m_BufferData.size = bufferSize;
		m_BufferData.state = bufferState;
	}

	VulkanIndexBuffer::VulkanIndexBuffer(std::vector<uint32_t>& indices, BufferState bufferState)
		: IndexBuffer(indices), m_Count(static_cast<uint32_t>(indices.size())), m_Indices(indices.data()), m_Size(static_cast<uint32_t>(sizeof(uint32_t) * indices.size()))
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();
		m_ID = vkBackends->vertexBufferCountIDs.take_next();

		VkDeviceSize bufferSize = static_cast<VkDeviceSize>(sizeof(vertex) * indices.size());
		m_IndexBufferSize = bufferSize;

		// If buffer is set to dynamic, we skip the staging buffer part and instead use the buffer on CPU memory
		m_UseStaging = bufferState != BufferState::Dynamic && bufferState != BufferState::DynamicResize;

		VmaMemoryUsage bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		if (m_UseStaging) bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		else bufferMemoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

		if (m_UseStaging) // create the staging buffer to copy data
		{
			vks::createBuffer(bufferSize, m_StagingBuffer, m_StagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

			if (!indices.empty())
			{
				void* data;
				vmaMapMemory(vmaAllocator, m_StagingBufferMemory, &data);
				std::memcpy(data, indices.data(), bufferSize);
				vmaUnmapMemory(vmaAllocator, m_StagingBufferMemory);
			}
		}

		// Create the main buffer used for binding
		vks::createBuffer(bufferSize, m_IndexBuffer, m_IndexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferMemoryUsage);

		// Copy the staging buffer to the main buffer if using staging
		if (m_UseStaging) vks::copyBuffer(m_StagingBuffer, m_IndexBuffer, bufferSize);
		else
		{
			if (!indices.empty())
			{
				void* data;
				vmaMapMemory(vmaAllocator, m_IndexBufferMemory, &data);
				std::memcpy(data, indices.data(), bufferSize);
				vmaUnmapMemory(vmaAllocator, m_IndexBufferMemory);
			}
		}

		m_BufferData.buffer = m_IndexBuffer;
		m_BufferData.staging = m_StagingBuffer;
		m_BufferData.size = bufferSize;
		m_BufferData.state = bufferState;
	}

	void VulkanIndexBuffer::Bind() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		vkCmdBindIndexBuffer(vkBackends->commandBuffers[vkBackends->currentFrame], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanIndexBuffer::Unbind() const
	{

	}

	void VulkanIndexBuffer::Delete()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		vkDestroyBuffer(vkBackends->device, m_IndexBuffer, nullptr);
		vkDestroyBuffer(vkBackends->device, m_StagingBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, m_IndexBufferMemory);
		vmaFreeMemory(vmaAllocator, m_StagingBufferMemory);

		vkBackends->indexBufferCountIDs.push(m_ID);
	}

	void VulkanIndexBuffer::Subdata(uint32_t* indices, uint32_t size)
	{
		CHONPS_CORE_ASSERT(m_BufferData.state != BufferState::Static, "Cannot subdata vertex buffer that has a static state!");

		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		m_Count = size / sizeof(float);

		if (m_BufferData.state == BufferState::DynamicResize && size > m_Size)
		{
			VmaMemoryUsage bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			if (m_UseStaging) bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			else bufferMemoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

			if (m_UseStaging)
			{
				vkDestroyBuffer(vkBackends->device, m_StagingBuffer, nullptr);
				vmaFreeMemory(vmaAllocator, m_StagingBufferMemory);
				vks::createBuffer(size, m_StagingBuffer, m_StagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
			}

			vkDestroyBuffer(vkBackends->device, m_IndexBuffer, nullptr);
			vmaFreeMemory(vmaAllocator, m_IndexBufferMemory);
			vks::createBuffer(size, m_IndexBuffer, m_IndexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferMemoryUsage);

			m_Size = static_cast<uint32_t>(size);
		}
		else CHONPS_CORE_ASSERT(size <= m_Size, "Vertex Buffer cannot change size unless it is set to DynamicResize!");

		if (m_UseStaging)
		{
			void* data;
			vmaMapMemory(vmaAllocator, m_StagingBufferMemory, &data);
			std::memcpy(data, indices, size);
			vmaUnmapMemory(vmaAllocator, m_StagingBufferMemory);
			vks::copyBuffer(m_StagingBuffer, m_IndexBuffer, m_Size);
		}
		else
		{
			void* data;
			vmaMapMemory(vmaAllocator, m_IndexBufferMemory, &data);
			std::memcpy(data, indices, size);
			vmaUnmapMemory(vmaAllocator, m_IndexBufferMemory);
		}
	}

	void VulkanIndexBuffer::Subdata(std::vector<uint32_t>& indices)
	{
		CHONPS_CORE_ASSERT(m_BufferData.state != BufferState::Static, "Cannot subdata vertex buffer that has a static state!");

		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		m_Count = static_cast<uint32_t>(indices.size());
		VkDeviceSize vertexSize = static_cast<VkDeviceSize>(sizeof(vertex) * indices.size());

		if (m_BufferData.state == BufferState::DynamicResize && vertexSize > m_Size)
		{
			VmaMemoryUsage bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			if (m_UseStaging) bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			else bufferMemoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

			if (m_UseStaging)
			{
				vkDestroyBuffer(vkBackends->device, m_StagingBuffer, nullptr);
				vmaFreeMemory(vmaAllocator, m_StagingBufferMemory);
				vks::createBuffer(vertexSize, m_StagingBuffer, m_StagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
			}

			vkDestroyBuffer(vkBackends->device, m_IndexBuffer, nullptr);
			vmaFreeMemory(vmaAllocator, m_IndexBufferMemory);
			vks::createBuffer(vertexSize, m_IndexBuffer, m_IndexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferMemoryUsage);

			m_Size = static_cast<uint32_t>(vertexSize);
		}
		else CHONPS_CORE_ASSERT(vertexSize <= m_Size, "Vertex Buffer cannot change size unless it is set to DynamicResize!");

		if (m_UseStaging)
		{
			void* data;
			vmaMapMemory(vmaAllocator, m_StagingBufferMemory, &data);
			std::memcpy(data, indices.data(), vertexSize);
			vmaUnmapMemory(vmaAllocator, m_StagingBufferMemory);
			vks::copyBuffer(m_StagingBuffer, m_IndexBuffer, m_Size);
		}
		else
		{
			void* data;
			vmaMapMemory(vmaAllocator, m_IndexBufferMemory, &data);
			std::memcpy(data, indices.data(), vertexSize);
			vmaUnmapMemory(vmaAllocator, m_IndexBufferMemory);
		}
	}
}