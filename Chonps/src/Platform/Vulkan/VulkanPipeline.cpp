#include "cepch.h"
#include "VulkanPipeline.h"

#include "Graphics/RendererBackends.h"

namespace Chonps
{
	static PipelineSpecification s_StandardPipelineSpecification;

	VkPipelineColorBlendAttachmentState createColorAttachment()
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = true;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		return colorBlendAttachment;
	}

	void vks::initStandardVulkanPipelineSpecification()
	{
		// Input Assembly
		s_StandardPipelineSpecification.inputAssembly.topology = RenderTopologyType::Triangle;
		s_StandardPipelineSpecification.inputAssembly.primitiveRestartEnable = false;

		// Viewport
		s_StandardPipelineSpecification.viewport.x = 0.0f;
		s_StandardPipelineSpecification.viewport.y = 0.0f;
		s_StandardPipelineSpecification.viewport.width = static_cast<float>(getVulkanBackends()->swapChainExtent.width);
		s_StandardPipelineSpecification.viewport.height = static_cast<float>(getVulkanBackends()->swapChainExtent.height);
		s_StandardPipelineSpecification.viewport.minDepth = 0.0f;
		s_StandardPipelineSpecification.viewport.maxDepth = 1.0f;

		// Scissor
		s_StandardPipelineSpecification.viewport.scissorsOffset = { 0, 0 };

		// Rasterizer
		s_StandardPipelineSpecification.rasterizer.depthClampEnable = false;
		s_StandardPipelineSpecification.rasterizer.rasterizerDiscardEnable = false;
		s_StandardPipelineSpecification.rasterizer.lineWidth = 1.0f;
		s_StandardPipelineSpecification.rasterizer.cullMode = RenderCullFaceMode::Back;
		s_StandardPipelineSpecification.rasterizer.frontFace = RenderCullFrontFace::CounterClockwise;
		s_StandardPipelineSpecification.rasterizer.depthBiasEnable = false;
		s_StandardPipelineSpecification.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		s_StandardPipelineSpecification.rasterizer.depthBiasClamp = 0.0f; // Optional
		s_StandardPipelineSpecification.rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		// MultiSampling
		s_StandardPipelineSpecification.multisampling.sampleShadingEnable = false;
		s_StandardPipelineSpecification.multisampling.rasterizationSamples = Sample::SampleCount_1_Bit;
		s_StandardPipelineSpecification.multisampling.minSampleShading = 1.0f; // Optional
		s_StandardPipelineSpecification.multisampling.sampleMask = nullptr; // Optional
		s_StandardPipelineSpecification.multisampling.alphaToCoverageEnable = false; // Optional
		s_StandardPipelineSpecification.multisampling.alphaToOneEnable = false; // Optional

		// Color Attachments
		s_StandardPipelineSpecification.colorBlend.colorBlendAttachmentCount = 0; // If no attachments are provided, an attachment will automatically be created
		s_StandardPipelineSpecification.colorBlend.pColorBlendAttachments = nullptr; 

		// Color Blend
		s_StandardPipelineSpecification.colorBlend.logicOpEnable = false;
		s_StandardPipelineSpecification.colorBlend.blendConstants[0] = 0.0f; // Optional
		s_StandardPipelineSpecification.colorBlend.blendConstants[1] = 0.0f; // Optional
		s_StandardPipelineSpecification.colorBlend.blendConstants[2] = 0.0f; // Optional
		s_StandardPipelineSpecification.colorBlend.blendConstants[3] = 0.0f; // Optional

		// Depth Stencil
		s_StandardPipelineSpecification.depthstencil.enableDepth = false;
		s_StandardPipelineSpecification.depthstencil.depthOpCompare = CompareOperation::Never;
		s_StandardPipelineSpecification.depthstencil.enableStencil = false;
		s_StandardPipelineSpecification.depthstencil.stencil.compareMask = 0x00;
		s_StandardPipelineSpecification.depthstencil.stencil.writeMask = 0x00;
		s_StandardPipelineSpecification.depthstencil.stencil.reference = 0;
		s_StandardPipelineSpecification.depthstencil.stencil.compareOp = CompareOperation::Never;
		s_StandardPipelineSpecification.depthstencil.stencil.depthFailOp = StencilOperation::Keep;
		s_StandardPipelineSpecification.depthstencil.stencil.failOp = StencilOperation::Keep;
		s_StandardPipelineSpecification.depthstencil.stencil.passOp = StencilOperation::Keep;
	}

	void vks::vkImplSetStandardVulkanPipelineSpecification(const PipelineSpecification& pipelineSpecification)
	{
		s_StandardPipelineSpecification = pipelineSpecification;
	}

	PipelineSpecification vks::vkImplGetStandardVulkanPipelineSpecification()
	{
		return s_StandardPipelineSpecification;
	}

	VulkanPipeline createVulkanPipeline(const PipelineSpecification& pipelineSpecification, VkPipelineShaderStageCreateInfo shaderStages[], VkPipelineVertexInputStateCreateInfo vertexInputInfo, VulkanPipelineLayoutData pipelineCreateData)
	{
		VulkanPipeline pipeline{};

		VulkanBackends* vkBackends = getVulkanBackends();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		
		inputAssembly.topology = vks::getVulkanTopologyType(pipelineSpecification.inputAssembly.topology);
		inputAssembly.primitiveRestartEnable = pipelineSpecification.inputAssembly.primitiveRestartEnable;
		
		VkViewport viewport{};
		viewport.x = pipelineSpecification.viewport.x;
		viewport.y = pipelineSpecification.viewport.y;
		pipelineSpecification.viewport.width < 0 ? viewport.width = (float)vkBackends->swapChainExtent.width : viewport.width = pipelineSpecification.viewport.width;
		pipelineSpecification.viewport.height < 0 ? viewport.height = (float)vkBackends->swapChainExtent.height : viewport.height = pipelineSpecification.viewport.height;
		viewport.minDepth = pipelineSpecification.viewport.minDepth;
		viewport.maxDepth = pipelineSpecification.viewport.maxDepth;

		VkRect2D scissor{};
		scissor.offset.x = pipelineSpecification.viewport.scissorsOffset.x;
		scissor.offset.y = pipelineSpecification.viewport.scissorsOffset.y;
		scissor.extent = vkBackends->swapChainExtent;

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};

		if (pipelineSpecification.depthstencil.enableStencil)
		{
			dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
			dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
			dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
		}

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = pipelineSpecification.rasterizer.depthClampEnable;
		rasterizer.rasterizerDiscardEnable = pipelineSpecification.rasterizer.rasterizerDiscardEnable;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = pipelineSpecification.rasterizer.lineWidth;
		rasterizer.cullMode = vks::getVulkanCullModeFlag(pipelineSpecification.rasterizer.cullMode);
		rasterizer.frontFace = vks::getVulkanCullFrontFace(pipelineSpecification.rasterizer.frontFace);
		rasterizer.depthBiasEnable = pipelineSpecification.rasterizer.depthBiasEnable;
		rasterizer.depthBiasConstantFactor = pipelineSpecification.rasterizer.depthBiasConstantFactor; // Optional
		rasterizer.depthBiasClamp = pipelineSpecification.rasterizer.depthBiasClamp; // Optional
		rasterizer.depthBiasSlopeFactor = pipelineSpecification.rasterizer.depthBiasSlopeFactor; // Optional

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = pipelineSpecification.multisampling.sampleShadingEnable;
		multisampling.rasterizationSamples = vks::getSupportedSampleCount(pipelineSpecification.multisampling.rasterizationSamples);
		multisampling.minSampleShading = pipelineSpecification.multisampling.minSampleShading; // Optional
		multisampling.pSampleMask = pipelineSpecification.multisampling.sampleMask; // Optional
		multisampling.alphaToCoverageEnable = pipelineSpecification.multisampling.alphaToCoverageEnable; // Optional
		multisampling.alphaToOneEnable = pipelineSpecification.multisampling.alphaToOneEnable; // Optional

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		
		if (pipelineSpecification.colorBlend.colorBlendAttachmentCount == 0 || pipelineSpecification.colorBlend.pColorBlendAttachments == nullptr)
		{
			colorBlendAttachments.push_back(createColorAttachment());
		}
		else
		{
			for (uint32_t i = 0; i < pipelineSpecification.colorBlend.colorBlendAttachmentCount; i++)
			{
				ColorBlendAttachment attachment = pipelineSpecification.colorBlend.pColorBlendAttachments[i];

				VkPipelineColorBlendAttachmentState colorBlendAttachment{};
				colorBlendAttachment.colorWriteMask = vks::getColorComponents(attachment.colorWriteMask);
				colorBlendAttachment.blendEnable = attachment.blendEnable;
				colorBlendAttachment.srcColorBlendFactor = vks::getBlendFactor(attachment.srcColorBlendFactor);
				colorBlendAttachment.dstColorBlendFactor = vks::getBlendFactor(attachment.dstColorBlendFactor);
				colorBlendAttachment.colorBlendOp = vks::getBlendOperation(attachment.colorBlendOp);
				colorBlendAttachment.srcAlphaBlendFactor = vks::getBlendFactor(attachment.srcAlphaBlendFactor);
				colorBlendAttachment.dstAlphaBlendFactor = vks::getBlendFactor(attachment.dstAlphaBlendFactor);
				colorBlendAttachment.alphaBlendOp = vks::getBlendOperation(attachment.alphaBlendOp);

				colorBlendAttachments.push_back(colorBlendAttachment);
			}
		}

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = pipelineSpecification.colorBlend.logicOpEnable;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
		colorBlending.pAttachments = colorBlendAttachments.data();
		colorBlending.blendConstants[0] = pipelineSpecification.colorBlend.blendConstants[0]; // Optional
		colorBlending.blendConstants[1] = pipelineSpecification.colorBlend.blendConstants[1]; // Optional
		colorBlending.blendConstants[2] = pipelineSpecification.colorBlend.blendConstants[2]; // Optional
		colorBlending.blendConstants[3] = pipelineSpecification.colorBlend.blendConstants[3]; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = pipelineSpecification.depthstencil.enableDepth ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = pipelineSpecification.depthstencil.enableDepth ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = vks::getCompareOp(pipelineSpecification.depthstencil.depthOpCompare);
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = pipelineSpecification.depthstencil.enableStencil ? VK_TRUE : VK_FALSE;
		depthStencil.back.compareMask = pipelineSpecification.depthstencil.stencil.compareMask;
		depthStencil.back.writeMask = pipelineSpecification.depthstencil.stencil.writeMask;
		depthStencil.back.reference = pipelineSpecification.depthstencil.stencil.reference;
		depthStencil.back.compareOp = vks::getCompareOp(pipelineSpecification.depthstencil.stencil.compareOp);
		depthStencil.back.depthFailOp = vks::getStencilOp(pipelineSpecification.depthstencil.stencil.depthFailOp);
		depthStencil.back.failOp = vks::getStencilOp(pipelineSpecification.depthstencil.stencil.failOp);
		depthStencil.back.passOp = vks::getStencilOp(pipelineSpecification.depthstencil.stencil.passOp);
		depthStencil.front = depthStencil.back;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		if (pipelineCreateData.descriptorSetLayoutCount != 0)
		{
			pipelineLayoutInfo.setLayoutCount = pipelineCreateData.descriptorSetLayoutCount;
			pipelineLayoutInfo.pSetLayouts = pipelineCreateData.pDescrptorSetLayouts;
		}
		else
		{
			pipelineLayoutInfo.setLayoutCount = 0;
			pipelineLayoutInfo.pSetLayouts = nullptr;
		}

		if (pipelineCreateData.pushConstantCount != 0)
		{
			pipelineLayoutInfo.pushConstantRangeCount = pipelineCreateData.pushConstantCount;
			pipelineLayoutInfo.pPushConstantRanges = pipelineCreateData.pPushConstants;
		}
		else
		{
			pipelineLayoutInfo.pushConstantRangeCount = 0;
			pipelineLayoutInfo.pPushConstantRanges = nullptr;
		}

		CHONPS_CORE_ASSERT(vkCreatePipelineLayout(vkBackends->device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) == VK_SUCCESS, "Failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.renderPass = pipelineCreateData.renderPass;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipeline.pipelineLayout;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		CHONPS_CORE_ASSERT(vkCreateGraphicsPipelines(vkBackends->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) == VK_SUCCESS, "Failed to create graphics pipeline!");

		return pipeline;
	}
}