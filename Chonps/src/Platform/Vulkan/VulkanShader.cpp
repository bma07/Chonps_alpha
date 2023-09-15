#include "cepch.h"
#include "VulkanShader.h"

#include "VulkanRendererAPI.h"
#include "VulkanVertexBuffer.h"
#include "VulkanVertexArray.h"

namespace Chonps
{
	VulkanShader::VulkanShader(const std::string& vertexFile, const std::string& fragmentFile, PipelineLayoutInfo* pipelineInfo)
		: Shader(vertexFile, fragmentFile, pipelineInfo)
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		CHONPS_CORE_ASSERT(!vkBackends->shaderCountIDs.empty(), "Reached max Shader count IDs!");

		m_ID = vkBackends->shaderCountIDs.take_next();

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

		BindPipeline(pipelineInfo);
	}

	VulkanShader::~VulkanShader()
	{
		Delete();
	}

	void VulkanShader::Bind() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();
		glfwGetFramebufferSize(vkBackends->currentWindow, &vkBackends->windowWidth, &vkBackends->windowHeight);
		if (vkBackends->windowWidth == 0 || vkBackends->windowHeight == 0)
			return;

		vkCmdBindPipeline(vkBackends->commandBuffers[vkBackends->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline.pipeline);
		vkBackends->currentBindedGraphicsPipeline = m_ID;

		if (m_DynamicStencil.enabled)
		{
			vkCmdSetStencilReference(vkBackends->commandBuffers[vkBackends->currentFrame], VK_STENCIL_FACE_FRONT_AND_BACK, m_DynamicStencil.reference);
			vkCmdSetStencilCompareMask(vkBackends->commandBuffers[vkBackends->currentFrame], VK_STENCIL_FACE_FRONT_AND_BACK, m_DynamicStencil.compareMask);
			vkCmdSetStencilWriteMask(vkBackends->commandBuffers[vkBackends->currentFrame], VK_STENCIL_FACE_FRONT_AND_BACK, m_DynamicStencil.writeMask);
		}
	}

	void VulkanShader::Unbind() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		vkCmdBindPipeline(vkBackends->commandBuffers[vkBackends->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, VK_NULL_HANDLE);
		getVulkanBackends()->currentBindedGraphicsPipeline = 0;
	}

	void VulkanShader::Delete() const
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		vkDestroyShaderModule(vkBackends->device, m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(vkBackends->device, m_FragmentShaderModule, nullptr);

		vkDestroyPipeline(vkBackends->device, m_Pipeline.pipeline, nullptr);
		vkDestroyPipelineLayout(vkBackends->device, m_Pipeline.pipelineLayout, nullptr);
		
		vkBackends->shaderCountIDs.push(m_ID);
	}

	void VulkanShader::BindPipeline(PipelineLayoutInfo* pipelineLayout)
	{
		m_LayoutInfo = *pipelineLayout;

		VulkanBackends* vkBackends = getVulkanBackends();

		// Pipeline Spec
		PipelineSpecification pipelineSpec;
		if (pipelineLayout->pipelineSpecification == nullptr)
			pipelineSpec = vks::vkImplGetStandardVulkanPipelineSpecification();
		else
			pipelineSpec = *pipelineLayout->pipelineSpecification;

		// Stencil Rendering
		m_DynamicStencil.enabled = pipelineSpec.depthstencil.enableStencil;
		m_DynamicStencil.failOp = vks::getStencilOp(pipelineSpec.depthstencil.stencil.failOp);
		m_DynamicStencil.depthFailOp = vks::getStencilOp(pipelineSpec.depthstencil.stencil.depthFailOp);
		m_DynamicStencil.passOp = vks::getStencilOp(pipelineSpec.depthstencil.stencil.passOp);
		m_DynamicStencil.compareOp = vks::getCompareOp(pipelineSpec.depthstencil.stencil.compareOp);
		m_DynamicStencil.reference = pipelineSpec.depthstencil.stencil.reference;
		m_DynamicStencil.compareMask = pipelineSpec.depthstencil.stencil.compareMask;
		m_DynamicStencil.writeMask = pipelineSpec.depthstencil.stencil.writeMask;

		// Vertex Input Binding Description
		std::vector<VkVertexInputAttributeDescription> attributeDescrption;
		for (uint32_t j = 0; j < pipelineLayout->vertexLayoutLinkInfo.layoutCount; j++)
		{
			VertexLayout vertLayout = pipelineLayout->vertexLayoutLinkInfo.pLayouts[j];
			attributeDescrption.push_back(vks::getAttributeDescriptions(vertLayout.layout, vks::getShaderDataTypeConvertVulkan(vertLayout.type, vertLayout.numComponents), vertLayout.stride, vertLayout.offset));
		}

		VkVertexInputBindingDescription bindingDescription = vks::getBindingDescription(pipelineLayout->bindingDescription);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescrption.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescrption.data();

		// Pipeline Descriptor Sets and push constants
		VulkanPipelineLayoutData pipelineLayoutData{};
		uint32_t descriptorLayoutCount = pipelineLayout->bufferLayoutsCount + pipelineLayout->layoutIncludeCount;
		pipelineLayoutData.descriptorSetLayoutCount = descriptorLayoutCount;

		// Check Descriptor Layout index
		std::vector<uint32_t> setIndices;
		for (uint32_t j = 0; j < pipelineLayout->bufferLayoutsCount; j++)
			setIndices.push_back(pipelineLayout->pBufferLayouts[j].setIndex);
		for (uint32_t j = 0; j < pipelineLayout->layoutIncludeCount; j++)
			setIndices.push_back(pipelineLayout->pLayoutIncludes[j].setIndex);

		std::sort(setIndices.begin(), setIndices.end());
		for (uint32_t i = 0; i < setIndices.size() - 1; i++)
		{
			CHONPS_CORE_ASSERT(setIndices[i] != setIndices[i + 1], "vkImplCreatePipelines() - Descriptor layout index has the same set index to another layout! Two Descriptor layouts cannot have the same set Index!");
		}

		// Sort Descriptor Layouts
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts; descriptorSetLayouts.resize(descriptorLayoutCount);
		for (uint32_t j = 0; j < pipelineLayout->bufferLayoutsCount; j++)
		{
			CHONPS_CORE_ASSERT(pipelineLayout->pBufferLayouts[j].setIndex < descriptorSetLayouts.size(), "vkImplCreatePipelines() - Buffer layout index must be less than the total number of layouts included!");
			descriptorSetLayouts[pipelineLayout->pBufferLayouts[j].setIndex] = static_cast<VkDescriptorSetLayout>(pipelineLayout->pBufferLayouts[j].layoutData);
		}
		for (uint32_t j = 0; j < pipelineLayout->layoutIncludeCount; j++)
		{
			if (pipelineLayout->pLayoutIncludes[j].imageLayoutOption == ImageLayoutOption::TextureImages)
			{
				CHONPS_CORE_ASSERT(pipelineLayout->pLayoutIncludes[j].setIndex < descriptorSetLayouts.size(), "vkImplCreatePipelines() - Texture set index must be less than the total number of descriptor layouts binded to this pipeline starting from index 0.");
				descriptorSetLayouts[pipelineLayout->pLayoutIncludes[j].setIndex] = vkBackends->textureDescriptorSetLayout;
			}
			if (pipelineLayout->pLayoutIncludes[j].imageLayoutOption == ImageLayoutOption::FrameBufferImages)
			{
				CHONPS_CORE_ASSERT(pipelineLayout->pLayoutIncludes[j].setIndex < descriptorSetLayouts.size(), "vkImplCreatePipelines() - FrameBuffer set index must be less than the total number of descriptor layouts binded to this pipeline starting from index 0.");
				descriptorSetLayouts[pipelineLayout->pLayoutIncludes[j].setIndex] = vkBackends->frameBufferDescriptorSetLayout;
			}
			if (pipelineLayout->pLayoutIncludes[j].imageLayoutOption == ImageLayoutOption::CubemapImages)
			{
				CHONPS_CORE_ASSERT(pipelineLayout->pLayoutIncludes[j].setIndex < descriptorSetLayouts.size(), "vkImplCreatePipelines() - Cubemap set index must be less than the total number of descriptor layouts binded to this pipeline starting from index 0.");
				descriptorSetLayouts[pipelineLayout->pLayoutIncludes[j].setIndex] = vkBackends->cubemapDescriptorSetLayout;
			}
		}

		pipelineLayoutData.pDescrptorSetLayouts = descriptorSetLayouts.data();

		// Push Constants
		pipelineLayoutData.pushConstantCount = pipelineLayout->pushConstantCount;
		std::vector<VkPushConstantRange> pushConstants;
		for (uint32_t j = 0; j < pipelineLayout->pushConstantCount; j++)
			pushConstants.push_back(*static_cast<VkPushConstantRange*>(pipelineLayout->pPushConstants[j].pcData));

		pipelineLayoutData.pPushConstants = pushConstants.data();

		// Render Pass
		if (pipelineLayout->renderPass != nullptr)
			pipelineLayoutData.renderPass = static_cast<VkRenderPass>(*pipelineLayout->renderPass);
		else
			pipelineLayoutData.renderPass = vkBackends->renderPass;

		// Get Shader and Shader stages
		VkPipelineShaderStageCreateInfo pipelineShaderStageInfo[] = { m_ShaderStages.vertexShaderStage, m_ShaderStages.fragementShaderStage };

		m_Pipeline = createVulkanPipeline(pipelineSpec, pipelineShaderStageInfo, vertexInputInfo, pipelineLayoutData);
		vkBackends->graphicsPipeline[m_ID] = m_Pipeline;
	}
}