#include "cepch.h"
#include "VulkanVertexBuffer.h"

namespace Chonps
{
	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size, BufferState bufferState)
		: VertexBuffer(vertices, size, bufferState), m_Count(size / sizeof(float)), m_Vertices(vertices), m_Size(size)
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();
		m_ID = vkBackends->vertexBufferCountIDs.take_next();

		VkDeviceSize bufferSize = size;
		m_VertexBufferSize = bufferSize;

		// If buffer is set to dynamic, we skip the staging buffer part and instead use the buffer on CPU memory
		m_UseStaging = bufferState != BufferState::Dynamic && bufferState != BufferState::DynamicResize;

		VmaMemoryUsage bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		if (m_UseStaging) bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		else bufferMemoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

		if (m_UseStaging) // create the staging buffer to copy data
		{
			vks::createBuffer(bufferSize, m_StagingBuffer, m_StagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

			if (vertices != nullptr)
			{
				void* data;
				vmaMapMemory(vmaAllocator, m_StagingBufferMemory, &data);
				std::memcpy(data, vertices, bufferSize);
				vmaUnmapMemory(vmaAllocator, m_StagingBufferMemory);
			}
		}

		// Create the main buffer used for binding
		vks::createBuffer(bufferSize, m_VertexBuffer, m_VertexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferMemoryUsage);
		
		// Copy the staging buffer to the main buffer if using staging
		if (m_UseStaging) vks::copyBuffer(m_StagingBuffer, m_VertexBuffer, bufferSize);
		else
		{
			if (vertices != nullptr)
			{
				void* data;
				vmaMapMemory(vmaAllocator, m_VertexBufferMemory, &data);
				std::memcpy(data, vertices, bufferSize);
				vmaUnmapMemory(vmaAllocator, m_VertexBufferMemory);
			}
		}

		m_BufferData.buffer = m_VertexBuffer;
		m_BufferData.staging = m_StagingBuffer;
		m_BufferData.size = bufferSize;
		m_BufferData.state = bufferState;
	}

	VulkanVertexBuffer::VulkanVertexBuffer(std::vector<vertex>& vertices, BufferState bufferState)
		: VertexBuffer(vertices, bufferState), m_Count(static_cast<uint32_t>(vertices.size())), m_Vertices((float*)vertices.data()), m_Size(static_cast<uint32_t>(sizeof(vertex)* vertices.size()))
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();
		m_ID = vkBackends->vertexBufferCountIDs.take_next();

		VkDeviceSize bufferSize = static_cast<VkDeviceSize>(sizeof(vertex) * vertices.size());
		m_VertexBufferSize = bufferSize;

		// If buffer is set to dynamic, we skip the staging buffer part and instead use the buffer on CPU memory
		m_UseStaging = bufferState != BufferState::Dynamic && bufferState != BufferState::DynamicResize;

		VmaMemoryUsage bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		if (m_UseStaging) bufferMemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		else bufferMemoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

		if (m_UseStaging) // create the staging buffer to copy data
		{
			vks::createBuffer(bufferSize, m_StagingBuffer, m_StagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

			if (!vertices.empty())
			{
				void* data;
				vmaMapMemory(vmaAllocator, m_StagingBufferMemory, &data);
				std::memcpy(data, vertices.data(), bufferSize);
				vmaUnmapMemory(vmaAllocator, m_StagingBufferMemory);
			}
		}

		// Create the main buffer used for binding
		vks::createBuffer(bufferSize, m_VertexBuffer, m_VertexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferMemoryUsage);

		// Copy the staging buffer to the main buffer if using staging
		if (m_UseStaging) vks::copyBuffer(m_StagingBuffer, m_VertexBuffer, bufferSize);
		else
		{
			if (!vertices.empty())
			{
				void* data;
				vmaMapMemory(vmaAllocator, m_VertexBufferMemory, &data);
				std::memcpy(data, vertices.data(), bufferSize);
				vmaUnmapMemory(vmaAllocator, m_VertexBufferMemory);
			}
		}

		m_BufferData.buffer = m_VertexBuffer;
		m_BufferData.staging = m_StagingBuffer;
		m_BufferData.size = bufferSize;
		m_BufferData.state = bufferState;
	}

	void VulkanVertexBuffer::Bind() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(vkBackends->commandBuffers[vkBackends->currentFrame], 0, 1, &m_VertexBuffer, offsets);
	}

	void VulkanVertexBuffer::Unbind() const
	{

	}

	void VulkanVertexBuffer::Delete()
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		vkDestroyBuffer(vkBackends->device, m_VertexBuffer, nullptr);
		vkDestroyBuffer(vkBackends->device, m_StagingBuffer, nullptr);
		vmaFreeMemory(vmaAllocator, m_VertexBufferMemory);
		vmaFreeMemory(vmaAllocator, m_StagingBufferMemory);

		vkBackends->vertexBufferCountIDs.push(m_ID);
	}

	void VulkanVertexBuffer::Subdata(float* vertices, uint32_t size)
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

			vkDestroyBuffer(vkBackends->device, m_VertexBuffer, nullptr);
			vmaFreeMemory(vmaAllocator, m_VertexBufferMemory);
			vks::createBuffer(size, m_VertexBuffer, m_VertexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferMemoryUsage);

			m_Size = static_cast<uint32_t>(size);
		}
		else CHONPS_CORE_ASSERT(size <= m_Size, "Vertex Buffer cannot change size unless it is set to DynamicResize!");

		if (m_UseStaging)
		{
			void* data;
			vmaMapMemory(vmaAllocator, m_StagingBufferMemory, &data);
			std::memcpy(data, vertices, size);
			vmaUnmapMemory(vmaAllocator, m_StagingBufferMemory);
			vks::copyBuffer(m_StagingBuffer, m_VertexBuffer, m_Size);
		}
		else
		{
			void* data;
			vmaMapMemory(vmaAllocator, m_VertexBufferMemory, &data);
			std::memcpy(data, vertices, size);
			vmaUnmapMemory(vmaAllocator, m_VertexBufferMemory);
		}
	}

	void VulkanVertexBuffer::Subdata(std::vector<vertex>& vertices)
	{
		CHONPS_CORE_ASSERT(m_BufferData.state != BufferState::Static, "Cannot subdata vertex buffer that has a static state!");
		
		VulkanBackends* vkBackends = getVulkanBackends();
		VmaAllocator vmaAllocator = getVmaAllocator();

		m_Count = static_cast<uint32_t>(vertices.size());
		VkDeviceSize vertexSize = static_cast<VkDeviceSize>(sizeof(vertex) * vertices.size());

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

			vkDestroyBuffer(vkBackends->device, m_VertexBuffer, nullptr);
			vmaFreeMemory(vmaAllocator, m_VertexBufferMemory);
			vks::createBuffer(vertexSize, m_VertexBuffer, m_VertexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferMemoryUsage);

			m_Size = static_cast<uint32_t>(vertexSize);
		}
		else CHONPS_CORE_ASSERT(vertexSize <= m_Size, "Vertex Buffer cannot change size unless it is set to DynamicResize!");

		if (m_UseStaging)
		{
			void* data;
			vmaMapMemory(vmaAllocator, m_StagingBufferMemory, &data);
			std::memcpy(data, vertices.data(), vertexSize);
			vmaUnmapMemory(vmaAllocator, m_StagingBufferMemory);
			vks::copyBuffer(m_StagingBuffer, m_VertexBuffer, m_Size);
		}
		else
		{
			void* data;
			vmaMapMemory(vmaAllocator, m_VertexBufferMemory, &data);
			std::memcpy(data, vertices.data(), vertexSize);
			vmaUnmapMemory(vmaAllocator, m_VertexBufferMemory);
		}
	}
}