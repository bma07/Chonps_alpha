#ifndef HG_CHONPS_VULKAN_PIPELINE_H
#define HG_CHONPS_VULKAN_PIPELINE_H

#include "Graphics/Pipeline.h"
#include "VulkanRendererAPI.h"

#include <vulkan/vulkan.h>

namespace Chonps
{
	VulkanPipeline createVulkanPipeline(const PipelineSpecification& pipelineSpecification, VkPipelineShaderStageCreateInfo shaderStages[], VkPipelineVertexInputStateCreateInfo vertexInputInfo, VulkanPipelineLayoutData pipelineCreateData);

	namespace vks
	{
		// Initialize a Vulkan pipeline specification with standard values set that can be reused across multiple pipelines for shaders
		// Make sure that the Pipeline Specification is initialized when the RendererAPI is initialized or it may cause errors when retrieving and copying the pipeline to somewhere such as to a Shader
		void initStandardVulkanPipelineSpecification();

		// Set a Vulkan pipeline specification that can be reused for multiple pipelines
		void vkImplSetStandardVulkanPipelineSpecification(const PipelineSpecification& pipelineSpecification);

		// Get the standard Vulkan pipeline specification that can be reused for multiple pipelines
		PipelineSpecification vkImplGetStandardVulkanPipelineSpecification();
	}
}

#endif