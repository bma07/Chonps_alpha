#include "cepch.h"
#include "VulkanShader.h"

#include "VulkanRendererAPI.h"
#include "VulkanVertexBuffer.h"

namespace Chonps
{
	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexFile, const std::string& fragmentFile)
		: Shader(name, vertexFile, fragmentFile), m_Name(name)
	{
		// Vertex File Loading
		VkShaderModule vertexShaderModule;
		std::vector<char> vertRaw = get_file_bin(vertexFile);
		VkShaderModuleCreateInfo createInfoVert{};
		createInfoVert.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfoVert.codeSize = vertRaw.size();
		createInfoVert.pCode = reinterpret_cast<const uint32_t*>(vertRaw.data());
		CHONPS_CORE_ASSERT(vkCreateShaderModule(getVulkanBackends()->device, &createInfoVert, nullptr, &vertexShaderModule) == VK_SUCCESS, "Failed to create shader module!");

		// Fragmemt File Loading
		VkShaderModule fragmentShaderModule;
		std::vector<char> fragRaw = get_file_bin(fragmentFile);
		VkShaderModuleCreateInfo createInfoFrag{};
		createInfoFrag.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfoFrag.codeSize = fragRaw.size();
		createInfoFrag.pCode = reinterpret_cast<const uint32_t*>(fragRaw.data());
		CHONPS_CORE_ASSERT(vkCreateShaderModule(getVulkanBackends()->device, &createInfoFrag, nullptr, &fragmentShaderModule) == VK_SUCCESS, "Failed to create shader module!");

		// Vertex Creation
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertexShaderModule;
		vertShaderStageInfo.pName = "main";

		// Fragment Creation
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragmentShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		auto bindingDescription = vkSpec::getBindingDescription();
		getVulkanBackends()->bindingDescriptions = bindingDescription;
		
		vkSpec::PipelineSpecification pipeline = vkSpec::getStandardVulkanPipelineSpecification();

		m_GraphicsPipeline = createVulkanPipeline(pipeline, shaderStages, vertexInputInfo);

		vkDestroyShaderModule(getVulkanBackends()->device, vertexShaderModule, nullptr);
		vkDestroyShaderModule(getVulkanBackends()->device, fragmentShaderModule, nullptr);
	}

	VulkanShader::VulkanShader(const std::string& filepath)
		: Shader(filepath)
	{
		
	}

	VulkanShader::~VulkanShader()
	{
		Delete();
	}

	void VulkanShader::Bind() const
	{
		getVulkanBackends()->graphicsPipeline = m_GraphicsPipeline.pipeline;
		getVulkanBackends()->pipelineLayout = m_GraphicsPipeline.pipelineLayout;
	}

	void VulkanShader::Unbind() const
	{
		// vkCmdBindPipeline(getVulkanBackends()->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 0);
	}

	void VulkanShader::Delete() const
	{
		VkDevice device = getVulkanBackends()->device;
		vkDestroyPipeline(device, m_GraphicsPipeline.pipeline, nullptr);
		vkDestroyPipelineLayout(device, m_GraphicsPipeline.pipelineLayout, nullptr);
	}
}