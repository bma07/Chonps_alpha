#include "cepch.h"
#include "VulkanPipeline.h"

#include "Graphics/RendererBackends.h"

namespace Chonps
{
	static PipelineSpecification s_StandardPipelineSpecification;

	VkPrimitiveTopology getVulkanTopologyType(RenderTopologyType topologyType)
	{
		switch (topologyType)
		{
			case RenderTopologyType::Point: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case RenderTopologyType::Line: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case RenderTopologyType::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case RenderTopologyType::Triangle: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case RenderTopologyType::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case RenderTopologyType::None: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
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

	VkColorComponentFlags getColorComponents(ColorWriteMask colorMask)
	{
		VkColorComponentFlags colorComponentsFlag = 0;

		if (colorMask.colorComponent_R_Bit) colorComponentsFlag |= VK_COLOR_COMPONENT_R_BIT;
		if (colorMask.colorComponent_G_Bit) colorComponentsFlag |= VK_COLOR_COMPONENT_G_BIT;
		if (colorMask.colorComponent_B_Bit) colorComponentsFlag |= VK_COLOR_COMPONENT_B_BIT;
		if (colorMask.colorComponent_A_Bit) colorComponentsFlag |= VK_COLOR_COMPONENT_A_BIT;

		return colorComponentsFlag;
	}

	VkBlendFactor getBlendFactor(ColorBlendFactor blendFactor)
	{
		switch (blendFactor)
		{
			case ColorBlendFactor::Zero: { return VK_BLEND_FACTOR_ZERO; }
			case ColorBlendFactor::One: { return VK_BLEND_FACTOR_ONE; }
			case ColorBlendFactor::SrcColor: { return VK_BLEND_FACTOR_SRC_COLOR; }
			case ColorBlendFactor::OneMinusSrcColor: { return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; }
			case ColorBlendFactor::DstColor: { return VK_BLEND_FACTOR_DST_COLOR; }
			case ColorBlendFactor::OneMinusDstColor: { return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; }
			case ColorBlendFactor::SrcAlpha: { return VK_BLEND_FACTOR_SRC_ALPHA; }
			case ColorBlendFactor::OneMinusSrcAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; }
			case ColorBlendFactor::DstAlpha: { return VK_BLEND_FACTOR_DST_ALPHA; }
			case ColorBlendFactor::OneMinusDstAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; }
			case ColorBlendFactor::ConstantColor: { return VK_BLEND_FACTOR_CONSTANT_COLOR; }
			case ColorBlendFactor::OneMinusConstantColor: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR; }
			case ColorBlendFactor::ConstantAlpha: { return VK_BLEND_FACTOR_CONSTANT_ALPHA; }
			case ColorBlendFactor::OneMinusConstantAlpha: { return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA; }
			case ColorBlendFactor::SrcAlphaSaturate: { return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; }
			case ColorBlendFactor::Src1Color: { return VK_BLEND_FACTOR_SRC1_COLOR; }
			case ColorBlendFactor::OneMinusSrc1Color: { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR; }
			case ColorBlendFactor::Src1_Alpha: { return VK_BLEND_FACTOR_SRC1_ALPHA; }
			case ColorBlendFactor::OneMinusSrc1Alpha: { return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA; }
			default:
			{
				CHONPS_CORE_ERROR("ERROR: PIPELINE: Could not find matching color blend factor!");
				return VK_BLEND_FACTOR_ZERO;
			}
		}
	}

	VkBlendOp getBlendOperation(ColorBlendOperation blendOp)
	{
		switch (blendOp)
		{
			case ColorBlendOperation::Add: { return VK_BLEND_OP_ADD; }
			case ColorBlendOperation::Subtract: { return VK_BLEND_OP_SUBTRACT; }
			case ColorBlendOperation::ReverseSubtract: { return VK_BLEND_OP_REVERSE_SUBTRACT; }
			case ColorBlendOperation::Min: { return VK_BLEND_OP_MIN; }
			case ColorBlendOperation::Max: { return VK_BLEND_OP_MAX; }
			case ColorBlendOperation::ZeroExt: { return VK_BLEND_OP_ZERO_EXT; }
			case ColorBlendOperation::SrcExt: { return VK_BLEND_OP_SRC_EXT; }
			case ColorBlendOperation::DstExt: { return VK_BLEND_OP_DST_EXT; }
			case ColorBlendOperation::SrcOverExt: { return VK_BLEND_OP_SRC_OVER_EXT; }
			case ColorBlendOperation::DstOverExt: { return VK_BLEND_OP_DST_OVER_EXT; }
			case ColorBlendOperation::SrcInExt: { return VK_BLEND_OP_SRC_IN_EXT; }
			case ColorBlendOperation::DstInExt: { return VK_BLEND_OP_DST_IN_EXT; }
			case ColorBlendOperation::SrcOutExt: { return VK_BLEND_OP_SRC_OUT_EXT; }
			case ColorBlendOperation::DstOutExt: { return VK_BLEND_OP_DST_OUT_EXT; }
			case ColorBlendOperation::SrcAtopExt: { return VK_BLEND_OP_SRC_ATOP_EXT; }
			case ColorBlendOperation::DstAtopExt: { return VK_BLEND_OP_DST_ATOP_EXT; }
			case ColorBlendOperation::XorExt: { return VK_BLEND_OP_XOR_EXT; }
			case ColorBlendOperation::MultiplyExt: { return VK_BLEND_OP_MULTIPLY_EXT; }
			case ColorBlendOperation::ScreenExt: { return VK_BLEND_OP_SCREEN_EXT; }
			case ColorBlendOperation::OverlayExt: { return VK_BLEND_OP_OVERLAY_EXT; }
			case ColorBlendOperation::DarkenExt: { return VK_BLEND_OP_DARKEN_EXT; }
			case ColorBlendOperation::LightenExt: { return VK_BLEND_OP_LIGHTEN_EXT; }
			case ColorBlendOperation::ColorDodgeExt: { return VK_BLEND_OP_COLORDODGE_EXT; }
			case ColorBlendOperation::ColorBurnExt: { return VK_BLEND_OP_COLORBURN_EXT; }
			case ColorBlendOperation::HardLightExt: { return VK_BLEND_OP_HARDLIGHT_EXT; }
			case ColorBlendOperation::SoftLightExt: { return VK_BLEND_OP_SOFTLIGHT_EXT; }
			case ColorBlendOperation::DifferenceExt: { return VK_BLEND_OP_DIFFERENCE_EXT; }
			case ColorBlendOperation::ExclusionExt: { return VK_BLEND_OP_EXCLUSION_EXT; }
			case ColorBlendOperation::InvertExt: { return VK_BLEND_OP_INVERT_EXT; }
			case ColorBlendOperation::InvertRGBExt: { return VK_BLEND_OP_INVERT_RGB_EXT; }
			case ColorBlendOperation::LinearDodgeExt: { return VK_BLEND_OP_LINEARDODGE_EXT; }
			case ColorBlendOperation::LinearBurnExt: { return VK_BLEND_OP_LINEARBURN_EXT; }
			case ColorBlendOperation::VividLightExt: { return VK_BLEND_OP_VIVIDLIGHT_EXT; }
			case ColorBlendOperation::LinearLightExt: { return VK_BLEND_OP_LINEARLIGHT_EXT; }
			case ColorBlendOperation::PinLightExt: { return VK_BLEND_OP_PINLIGHT_EXT; }
			case ColorBlendOperation::HardMixExt: { return VK_BLEND_OP_HARDMIX_EXT; }
			case ColorBlendOperation::HSLHueExt: { return VK_BLEND_OP_HSL_HUE_EXT; }
			case ColorBlendOperation::HSLSaturationExt: { return VK_BLEND_OP_HSL_SATURATION_EXT; }
			case ColorBlendOperation::HSLColorExt: { return VK_BLEND_OP_HSL_COLOR_EXT; }
			case ColorBlendOperation::HSLLuminosityExt: { return VK_BLEND_OP_HSL_LUMINOSITY_EXT; }
			case ColorBlendOperation::PlusExt: { return VK_BLEND_OP_PLUS_EXT; }
			case ColorBlendOperation::PlusClampedExt: { return VK_BLEND_OP_PLUS_CLAMPED_EXT; }
			case ColorBlendOperation::PlusClampedAlphaExt: { return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT; }
			case ColorBlendOperation::PlusDarkerExt: { return VK_BLEND_OP_PLUS_DARKER_EXT; }
			case ColorBlendOperation::MinusExt: { return VK_BLEND_OP_MINUS_EXT; }
			case ColorBlendOperation::MinusClampedExt: { return VK_BLEND_OP_MINUS_CLAMPED_EXT; }
			case ColorBlendOperation::ContrastExt: { return VK_BLEND_OP_CONTRAST_EXT; }
			case ColorBlendOperation::InvertOVGExt: { return VK_BLEND_OP_INVERT_OVG_EXT; }
			case ColorBlendOperation::RedExt: { return VK_BLEND_OP_RED_EXT; }
			case ColorBlendOperation::GreenExt: { return VK_BLEND_OP_GREEN_EXT; }
			case ColorBlendOperation::BlueExt: { return VK_BLEND_OP_BLUE_EXT; }
			default:
			{
				CHONPS_CORE_ERROR("ERROR: PIPELINE: Could not find matching blend operation!");
				return VK_BLEND_OP_ADD;
			}
		}
	}

	VkCompareOp getCompareOp(CompareOperation compare)
	{
		switch (compare)
		{
			case CompareOperation::Never: { return VK_COMPARE_OP_NEVER; }
			case CompareOperation::Less: { return VK_COMPARE_OP_LESS; }
			case CompareOperation::Equal: { return VK_COMPARE_OP_EQUAL; }
			case CompareOperation::LessOrEqual: { return VK_COMPARE_OP_LESS_OR_EQUAL; }
			case CompareOperation::Greater: { return VK_COMPARE_OP_GREATER; }
			case CompareOperation::NotEqual: { return VK_COMPARE_OP_NOT_EQUAL; }
			case CompareOperation::GreaterOrEqual: { return VK_COMPARE_OP_GREATER_OR_EQUAL; }
			case CompareOperation::Always: { return VK_COMPARE_OP_ALWAYS; }
			default:
			{
				CHONPS_CORE_ERROR("ERROR: PIPELINE: Could not find matching compare operation!");
				return VK_COMPARE_OP_NEVER;
			}
		}
	}

	VkStencilOp getStencilOp(StencilOperation stencilOp)
	{
		switch (stencilOp)
		{
			case StencilOperation::Keep: { return VK_STENCIL_OP_KEEP; }
			case StencilOperation::Zero: { return VK_STENCIL_OP_ZERO; }
			case StencilOperation::Replace: { return VK_STENCIL_OP_REPLACE; }
			case StencilOperation::IncrementAndClamp: { return VK_STENCIL_OP_INCREMENT_AND_CLAMP; }
			case StencilOperation::DecrementAndClamp: { return VK_STENCIL_OP_DECREMENT_AND_CLAMP; }
			case StencilOperation::Invert: { return VK_STENCIL_OP_INVERT; }
			case StencilOperation::IncrementAndWrap: { return VK_STENCIL_OP_INCREMENT_AND_WRAP; }
			case StencilOperation::DecrementAndWrap: { return VK_STENCIL_OP_DECREMENT_AND_WRAP; }
			default:
			{
				CHONPS_CORE_ERROR("ERROR: PIPELINE: Could not find matching stencil operation!");
				return VK_STENCIL_OP_KEEP;
			}
		}
	}

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
				colorBlendAttachment.colorWriteMask = getColorComponents(attachment.colorWriteMask);
				colorBlendAttachment.blendEnable = attachment.blendEnable;
				colorBlendAttachment.srcColorBlendFactor = getBlendFactor(attachment.srcColorBlendFactor);
				colorBlendAttachment.dstColorBlendFactor = getBlendFactor(attachment.dstColorBlendFactor);
				colorBlendAttachment.colorBlendOp = getBlendOperation(attachment.colorBlendOp);
				colorBlendAttachment.srcAlphaBlendFactor = getBlendFactor(attachment.srcAlphaBlendFactor);
				colorBlendAttachment.dstAlphaBlendFactor = getBlendFactor(attachment.dstAlphaBlendFactor);
				colorBlendAttachment.alphaBlendOp = getBlendOperation(attachment.alphaBlendOp);

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
		depthStencil.depthCompareOp = getCompareOp(pipelineSpecification.depthstencil.depthOpCompare);
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = pipelineSpecification.depthstencil.enableStencil ? VK_TRUE : VK_FALSE;
		depthStencil.back.compareMask = pipelineSpecification.depthstencil.stencil.compareMask;
		depthStencil.back.writeMask = pipelineSpecification.depthstencil.stencil.writeMask;
		depthStencil.back.reference = pipelineSpecification.depthstencil.stencil.reference;
		depthStencil.back.compareOp = getCompareOp(pipelineSpecification.depthstencil.stencil.compareOp);
		depthStencil.back.depthFailOp = getStencilOp(pipelineSpecification.depthstencil.stencil.depthFailOp);
		depthStencil.back.failOp = getStencilOp(pipelineSpecification.depthstencil.stencil.failOp);
		depthStencil.back.passOp = getStencilOp(pipelineSpecification.depthstencil.stencil.passOp);
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