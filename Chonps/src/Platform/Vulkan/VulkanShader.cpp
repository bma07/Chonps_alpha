#include "cepch.h"
#include "VulkanShader.h"

#include "VulkanRendererAPI.h"
#include "VulkanVertexBuffer.h"
#include "VulkanVertexArray.h"

#include <shaderc/shaderc.hpp>

namespace Chonps
{
	static bool s_VulkanUsePushConstant = false;
	static std::vector<VkPushConstantRange> s_VulkanPushConstantRange;

	namespace vks
	{
		void vkImplSetUsePushConstant(bool use)
		{
			s_VulkanUsePushConstant = use;
		}

		void vkImplSetPushConstantRange(PushConstantRange* pushConstant, uint32_t count)
		{
			if (pushConstant == nullptr || count == 0)
			{
				s_VulkanPushConstantRange.clear();
				return;
			}

			for (uint32_t i = 0; i < count; i++)
			{
				VkPushConstantRange pushConstantRange{};
				pushConstantRange.size = pushConstant[i].size;
				pushConstantRange.offset = pushConstant[i].offset;
				pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;

				switch (pushConstant[i].shaderStage)
				{
					case ShaderStage::All:
					{
						pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
						break;
					}
					case ShaderStage::Vertex:
					{
						pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
						break;
					}
					case ShaderStage::Fragment:
					{
						pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
						break;
					}
				}

				s_VulkanPushConstantRange.push_back(pushConstantRange);
			}
		}

		void vkImplRenderPushConstant(Shader* shader, ShaderStage shaderStage, uint32_t size, uint32_t offset, void* pValues)
		{
			VkShaderStageFlagBits stageFlags;

			switch (shaderStage)
			{
				case ShaderStage::All:
				{
					stageFlags = VK_SHADER_STAGE_ALL;
					break;
				}
				case ShaderStage::Vertex:
				{
					stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
					break;
				}
				case ShaderStage::Fragment:
				{
					stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				}
			}

			VulkanBackends* vkBackends = getVulkanBackends();

			VkPipelineLayout pipelineLayout = static_cast<VulkanShader*>(shader)->getNativePipelineLayout();
			vkCmdPushConstants(vkBackends->commandBuffers[vkBackends->currentFrame], pipelineLayout, stageFlags, offset, size, pValues);
		}
	}

	std::vector<uint32_t> vkImpl_compile_shader(const std::string& source_name, shaderc_shader_kind kind, const std::string& source, bool optimize = false)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

		shaderc::SpvCompilationResult spvModule =
			compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

		if (spvModule.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			CHONPS_CORE_LOG_ERROR(Vulkan:Shader:SPIRV, "{0}", spvModule.GetErrorMessage());
			return std::vector<uint32_t>();
		}

		return { spvModule.cbegin(), spvModule.cend() };
	}

	VulkanShader::VulkanShader(const std::string& vertex, const std::string& fragment, PipelineLayoutInfo* pipelineInfo)
		: Shader(vertex, fragment, pipelineInfo)
	{
		VulkanBackends* vkBackends = getVulkanBackends();

		CHONPS_CORE_ASSERT(!vkBackends->shaderCountIDs.empty(), "Reached max Shader count IDs!");

		m_ID = vkBackends->shaderCountIDs.take_next();

		std::vector<uint32_t> vertRaw = vkImpl_compile_shader("vertex", shaderc_vertex_shader, vertex);
		std::vector<uint32_t> fragRaw = vkImpl_compile_shader("fragment", shaderc_fragment_shader, fragment);

		// Vertex File Loading
		VkShaderModuleCreateInfo createInfoVert{};
		createInfoVert.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfoVert.codeSize = vertRaw.size() * sizeof(uint32_t);
		createInfoVert.pCode = vertRaw.data();
		CHONPS_CORE_ASSERT(vkCreateShaderModule(vkBackends->device, &createInfoVert, nullptr, &m_VertexShaderModule) == VK_SUCCESS, "Failed to create shader module!");

		// Fragmemt File Loading
		VkShaderModuleCreateInfo createInfoFrag{};
		createInfoFrag.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfoFrag.codeSize = fragRaw.size() * sizeof(uint32_t);
		createInfoFrag.pCode = fragRaw.data();
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
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts; descriptorSetLayouts.resize(descriptorLayoutCount);

		// Check Descriptor Layout index
		if (descriptorLayoutCount != 0)
		{
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
		}
		else
		{
			pipelineLayoutData.pDescrptorSetLayouts = nullptr;
		}

		// Push Constants
		if (s_VulkanUsePushConstant)
		{
			pipelineLayoutData.pushConstantCount = static_cast<uint32_t>(s_VulkanPushConstantRange.size());
			pipelineLayoutData.pPushConstants = s_VulkanPushConstantRange.data();
		}
		else
		{
			pipelineLayoutData.pushConstantCount = 0;
			pipelineLayoutData.pPushConstants = nullptr;
		}

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