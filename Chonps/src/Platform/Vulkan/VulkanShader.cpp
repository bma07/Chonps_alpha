#include "cepch.h"
#include "VulkanShader.h"

#include "VulkanRendererAPI.h"
#include "VulkanVertexBuffer.h"

namespace Chonps
{
	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexFile, const std::string& fragmentFile)
		: Shader(name, vertexFile, fragmentFile), m_Name(name)
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		CHONPS_CORE_ASSERT(!vkBackends->shaderCountIDs.empty(), "Reached max Shader count IDs!");

		m_ID = vkBackends->shaderCountIDs.front();
		vkBackends->shaderCountIDs.pop();

		// Vertex File Loading
		std::vector<char> vertRaw = get_file_bin(vertexFile);
		VkShaderModuleCreateInfo createInfoVert{};
		createInfoVert.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfoVert.codeSize = vertRaw.size();
		createInfoVert.pCode = reinterpret_cast<const uint32_t*>(vertRaw.data());
		CHONPS_CORE_ASSERT(vkCreateShaderModule(vkBackends->device, &createInfoVert, nullptr, &m_VertexShaderModule) == VK_SUCCESS, "Failed to create shader module!");

		// Fragmemt File Loading
		std::vector<char> fragRaw = get_file_bin(fragmentFile);
		VkShaderModuleCreateInfo createInfoFrag{};
		createInfoFrag.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfoFrag.codeSize = fragRaw.size();
		createInfoFrag.pCode = reinterpret_cast<const uint32_t*>(fragRaw.data());
		CHONPS_CORE_ASSERT(vkCreateShaderModule(vkBackends->device, &createInfoFrag, nullptr, &m_FragmentShaderModule) == VK_SUCCESS, "Failed to create shader module!");

		// Vertex Creation
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_VertexShaderModule;
		vertShaderStageInfo.pName = "main";

		// Fragment Creation
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_FragmentShaderModule;
		fragShaderStageInfo.pName = "main";

		m_ShaderStages.vertexShaderStage = vertShaderStageInfo;
		m_ShaderStages.fragementShaderStage = fragShaderStageInfo;

		vkBackends->pipelineShaderStages[m_ID] = m_ShaderStages;
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
		getVulkanBackends()->currentBindedGraphicsPipeline = m_ID;
	}

	void VulkanShader::Unbind() const
	{
		getVulkanBackends()->currentBindedGraphicsPipeline = 0;
	}

	void VulkanShader::Delete() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		vkDestroyShaderModule(vkBackends->device, m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(vkBackends->device, m_FragmentShaderModule, nullptr);

		vkBackends->pipelineShaderStages.erase(m_ID);
		vkBackends->shaderCountIDs.push(m_ID);
	}
}