#include "cepch.h"
#include "VulkanPipeline.h"

#include "Graphics/RendererBackends.h"

namespace Chonps
{
	static vkSpec::PipelineSpecification s_StandardPipelineSpecification;

	VkPrimitiveTopology getVulkanTopologyType(RenderTopologyType topologyType)
	{
		switch (topologyType)
		{
			case RenderTopologyType::Point: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case RenderTopologyType::Line: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case RenderTopologyType::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case RenderTopologyType::Triangle: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case RenderTopologyType::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case RenderTopologyType::Default: return getVulkanTopologyType(renderGetTopologyType());
			}

		CHONPS_CORE_ERROR("ERROR: VULKAN: No topology was chosen, cannot get topology type!");
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}

	VkCullModeFlags getVulkanCullModeFlag(RenderCullFaceMode cullFaceMode)
	{
		switch (cullFaceMode)
		{
			case RenderCullFaceMode::Disable: return VK_CULL_MODE_NONE;
			case RenderCullFaceMode::Front: return VK_CULL_MODE_FRONT_BIT;
			case RenderCullFaceMode::Back: return VK_CULL_MODE_BACK_BIT;
			case RenderCullFaceMode::Both: return VK_CULL_MODE_FRONT_AND_BACK;
		}

		CHONPS_CORE_ERROR("ERROR: VULKAN: No cull face mode was chosen, cannot get cull face mode!");
		return VK_CULL_MODE_NONE;
	}

	VkFrontFace getVulkanCullFrontFace(RenderCullFrontFace cullFrontFace)
	{
		switch (cullFrontFace)
		{
		case RenderCullFrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
		case RenderCullFrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}

		CHONPS_CORE_ERROR("ERROR: VULKAN: No cull front face was chosen, cannot get cull front face!");
		return VK_FRONT_FACE_CLOCKWISE;
	}

	VkSampleCountFlagBits getVulkanMultiSampeCount(vkSpec::Sample sampleCount)
	{
		switch (sampleCount)
		{
			case vkSpec::Sample::SampleCount_1_Bit: return VK_SAMPLE_COUNT_1_BIT;
			case vkSpec::Sample::SampleCount_2_Bit: return VK_SAMPLE_COUNT_2_BIT;
			case vkSpec::Sample::SampleCount_4_Bit: return VK_SAMPLE_COUNT_4_BIT;
			case vkSpec::Sample::SampleCount_8_Bit: return VK_SAMPLE_COUNT_8_BIT;
			case vkSpec::Sample::SampleCount_16_Bit: return VK_SAMPLE_COUNT_16_BIT;
			case vkSpec::Sample::SampleCount_32_Bit: return VK_SAMPLE_COUNT_32_BIT;
			case vkSpec::Sample::SampleCount_64_Bit: return VK_SAMPLE_COUNT_64_BIT;
			case vkSpec::Sample::SampleCount_Max_Bit: return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
		}

		CHONPS_CORE_ERROR("ERROR: VULKAN: No Sample count was chosen, cannot get Sample count!");
		return VK_SAMPLE_COUNT_1_BIT;
	}

	void vkSpec::initStandardVulkanPipelineSpecification()
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
		s_StandardPipelineSpecification.multisampling.rasterizationSamples = vkSpec::Sample::SampleCount_1_Bit;
		s_StandardPipelineSpecification.multisampling.minSampleShading = 1.0f; // Optional
		s_StandardPipelineSpecification.multisampling.sampleMask = nullptr; // Optional
		s_StandardPipelineSpecification.multisampling.alphaToCoverageEnable = false; // Optional
		s_StandardPipelineSpecification.multisampling.alphaToOneEnable = false; // Optional

		// Color Blend
		s_StandardPipelineSpecification.colorBlend.blendEnable = true;
		s_StandardPipelineSpecification.colorBlend.logicOpEnable = false;
		s_StandardPipelineSpecification.colorBlend.blendConstants[0] = 0.0f; // Optional
		s_StandardPipelineSpecification.colorBlend.blendConstants[1] = 0.0f; // Optional
		s_StandardPipelineSpecification.colorBlend.blendConstants[2] = 0.0f; // Optional
		s_StandardPipelineSpecification.colorBlend.blendConstants[3] = 0.0f; // Optional

		// Pipeline Layout
		s_StandardPipelineSpecification.pipelineLayout.setLayoutCount = 0; // Optional
		s_StandardPipelineSpecification.pipelineLayout.setLayouts = nullptr; // Optional
		s_StandardPipelineSpecification.pipelineLayout.pushConstantRangeCount = 0; // Optional
		s_StandardPipelineSpecification.pipelineLayout.pushConstantRanges = nullptr; // Optional
	}

	void vkSpec::setStandardVulkanPipelineSpecification(const vkSpec::PipelineSpecification& pipelineSpecification)
	{
		s_StandardPipelineSpecification = pipelineSpecification;
	}

	vkSpec::PipelineSpecification& vkSpec::getStandardVulkanPipelineSpecification()
	{
		return s_StandardPipelineSpecification;
	}

	VulkanPipeline createVulkanPipeline(const vkSpec::PipelineSpecification& pipelineSpecification, VkPipelineShaderStageCreateInfo shaderStages[], VkPipelineVertexInputStateCreateInfo vertexInputInfo)
	{
		VulkanPipeline pipeline{};

		VulkanBackends* vkBackends = getVulkanBackends();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		
		inputAssembly.topology = getVulkanTopologyType(pipelineSpecification.inputAssembly.topology);
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
			VK_DYNAMIC_STATE_VERTEX_INPUT_EXT
		};

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
		rasterizer.cullMode = getVulkanCullModeFlag(pipelineSpecification.rasterizer.cullMode);
		rasterizer.frontFace = getVulkanCullFrontFace(pipelineSpecification.rasterizer.frontFace);
		rasterizer.depthBiasEnable = pipelineSpecification.rasterizer.depthBiasEnable;
		rasterizer.depthBiasConstantFactor = pipelineSpecification.rasterizer.depthBiasConstantFactor; // Optional
		rasterizer.depthBiasClamp = pipelineSpecification.rasterizer.depthBiasClamp; // Optional
		rasterizer.depthBiasSlopeFactor = pipelineSpecification.rasterizer.depthBiasSlopeFactor; // Optional

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = pipelineSpecification.multisampling.sampleShadingEnable;
		multisampling.rasterizationSamples = getVulkanMultiSampeCount(pipelineSpecification.multisampling.rasterizationSamples);
		multisampling.minSampleShading = pipelineSpecification.multisampling.minSampleShading; // Optional
		multisampling.pSampleMask = pipelineSpecification.multisampling.sampleMask; // Optional
		multisampling.alphaToCoverageEnable = pipelineSpecification.multisampling.alphaToCoverageEnable; // Optional
		multisampling.alphaToOneEnable = pipelineSpecification.multisampling.alphaToOneEnable; // Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = pipelineSpecification.colorBlend.blendEnable;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = pipelineSpecification.colorBlend.logicOpEnable;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = pipelineSpecification.colorBlend.blendConstants[0]; // Optional
		colorBlending.blendConstants[1] = pipelineSpecification.colorBlend.blendConstants[1]; // Optional
		colorBlending.blendConstants[2] = pipelineSpecification.colorBlend.blendConstants[2]; // Optional
		colorBlending.blendConstants[3] = pipelineSpecification.colorBlend.blendConstants[3]; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		if (!vkBackends->toPipelineDescriptorSetLayouts.empty())
		{
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(vkBackends->toPipelineDescriptorSetLayouts.size());
			pipelineLayoutInfo.pSetLayouts = vkBackends->toPipelineDescriptorSetLayouts.data();
		}
		else
		{
			pipelineLayoutInfo.setLayoutCount = 0;
			pipelineLayoutInfo.pSetLayouts = nullptr;
		}

		if (vkBackends->pushConstantRangeCount > 0)
		{
			pipelineLayoutInfo.pushConstantRangeCount = vkBackends->pushConstantRangeCount;
			pipelineLayoutInfo.pPushConstantRanges = &vkBackends->pushConstantRange;
		}
		else
		{
			pipelineLayoutInfo.pushConstantRangeCount = 0;
			pipelineLayoutInfo.pPushConstantRanges = nullptr;
		}

		CHONPS_CORE_ASSERT(vkCreatePipelineLayout(vkBackends->device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) == VK_SUCCESS, "Failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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
		pipelineInfo.renderPass = vkBackends->renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		CHONPS_CORE_ASSERT(vkCreateGraphicsPipelines(vkBackends->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) == VK_SUCCESS, "Failed to create graphics pipeline!");

		return pipeline;
	}
}