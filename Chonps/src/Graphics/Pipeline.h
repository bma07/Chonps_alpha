#ifndef HG_CHONPS_PIPELINE_H
#define HG_CHONPS_PIPELINE_H

#include "VertexArray.h"
#include "UniformBuffer.h"

namespace Chonps
{
	enum class RenderTopologyType
	{
		None = 0,
		Point,
		Line,
		LineStrip,
		Triangle,
		TriangleStrip,
	};
	typedef RenderTopologyType TopologyType;

	enum class RenderCullFaceMode
	{
		Front,
		Back,
		Both,
		Disable
	};
	typedef RenderCullFaceMode CullFaceMode;

	enum class RenderCullFrontFace
	{
		Clockwise,
		CounterClockwise,

		CW = Clockwise,
		CCW = CounterClockwise
	};
	typedef RenderCullFrontFace CullFrontFace;

	typedef void* RenderPass;

	// Set topology to default if no topology type needs to be specified
	// topology type will be copied from RendererAPI's topology type
	struct InputAssembly
	{
		RenderTopologyType topology;
		bool primitiveRestartEnable;
	};

	struct rec2d
	{
		int x, y;
	};

	// width and height are based on GLFW window framebuffer size
	// Note: GLFW framebuffer size and window pixel coordinates may not be the same on different systems
	// Set width and height to -1 if it does not need to be specified, width and height will then be automatically set to the framebuffer size
	struct Viewport
	{
		float x, y;
		float width, height;
		float minDepth, maxDepth;

		rec2d scissorsOffset;
	};

	struct Rasterizer
	{
		bool depthClampEnable;
		bool rasterizerDiscardEnable;
		float lineWidth;

		RenderCullFaceMode cullMode = CullFaceMode::Disable;
		RenderCullFrontFace frontFace = CullFrontFace::CounterClockwise;

		bool depthBiasEnable;
		float depthBiasConstantFactor = 0.0f; // Optional
		float depthBiasClamp = 0.0f; // Optional
		float depthBiasSlopeFactor = 0.0f; // Optional
	};

	enum class Sample
	{
		SampleCount_1_Bit = 0x00000001,
		SampleCount_2_Bit = 0x00000002,
		SampleCount_4_Bit = 0x00000004,
		SampleCount_8_Bit = 0x00000008,
		SampleCount_16_Bit = 0x00000010,
		SampleCount_32_Bit = 0x00000020,
		SampleCount_64_Bit = 0x00000040,
		SampleCount_Max_Bit = 0x7FFFFFFF
	};

	struct MultiSampling
	{
		bool sampleShadingEnable;
		Sample rasterizationSamples;
		float minSampleShading = 1.0f; // Optional
		uint32_t* sampleMask = nullptr; // Optional
		bool alphaToCoverageEnable = false; // Optional
		bool alphaToOneEnable = false; // Optional
	};

	struct ColorWriteMask
	{
		bool colorComponent_R_Bit;
		bool colorComponent_G_Bit;
		bool colorComponent_B_Bit;
		bool colorComponent_A_Bit;
	};

	enum class ColorBlendFactor
	{
		Zero = 0,
		One = 1,
		SrcColor = 2,
		OneMinusSrcColor = 3,
		DstColor = 4,
		OneMinusDstColor = 5,
		SrcAlpha = 6,
		OneMinusSrcAlpha = 7,
		DstAlpha = 8,
		OneMinusDstAlpha = 9,
		ConstantColor = 10,
		OneMinusConstantColor = 11,
		ConstantAlpha = 12,
		OneMinusConstantAlpha = 13,
		SrcAlphaSaturate = 14,
		Src1Color = 15,
		OneMinusSrc1Color = 16,
		Src1_Alpha = 17,
		OneMinusSrc1Alpha = 18,
	};

	enum class ColorBlendOperation
	{
		Add = 0,
		Subtract = 1,
		ReverseSubtract = 2,
		Min = 3,
		Max = 4,
		ZeroExt = 1000148000,
		SrcExt = 1000148001,
		DstExt = 1000148002,
		SrcOverExt = 1000148003,
		DstOverExt = 1000148004,
		SrcInExt = 1000148005,
		DstInExt = 1000148006,
		SrcOutExt = 1000148007,
		DstOutExt = 1000148008,
		SrcAtopExt = 1000148009,
		DstAtopExt = 1000148010,
		XorExt = 1000148011,
		MultiplyExt = 1000148012,
		ScreenExt = 1000148013,
		OverlayExt = 1000148014,
		DarkenExt = 1000148015,
		LightenExt = 1000148016,
		ColorDodgeExt = 1000148017,
		ColorBurnExt = 1000148018,
		HardLightExt = 1000148019,
		SoftLightExt = 1000148020,
		DifferenceExt = 1000148021,
		ExclusionExt = 1000148022,
		InvertExt = 1000148023,
		InvertRGBExt = 1000148024,
		LinearDodgeExt = 1000148025,
		LinearBurnExt = 1000148026,
		VividLightExt = 1000148027,
		LinearLightExt = 1000148028,
		PinLightExt = 1000148029,
		HardMixExt = 1000148030,
		HSLHueExt = 1000148031,
		HSLSaturationExt = 1000148032,
		HSLColorExt = 1000148033,
		HSLLuminosityExt = 1000148034,
		PlusExt = 1000148035,
		PlusClampedExt = 1000148036,
		PlusClampedAlphaExt = 1000148037,
		PlusDarkerExt = 1000148038,
		MinusExt = 1000148039,
		MinusClampedExt = 1000148040,
		ContrastExt = 1000148041,
		InvertOVGExt = 1000148042,
		RedExt = 1000148043,
		GreenExt = 1000148044,
		BlueExt = 1000148045,
	};

	enum class CompareOperation
	{
		Never = 0,
		Less = 1,
		Equal = 2,
		LessOrEqual = 3,
		Greater = 4,
		NotEqual = 5,
		GreaterOrEqual = 6,
		Always = 7,
	};

	enum class StencilOperation
	{
		Keep = 0,
		Zero = 1,
		Replace = 2,
		IncrementAndClamp = 3,
		DecrementAndClamp = 4,
		Invert = 5,
		IncrementAndWrap = 6,
		DecrementAndWrap = 7,
	};

	struct ColorBlendAttachment
	{
		ColorWriteMask colorWriteMask;
		bool blendEnable;
		ColorBlendFactor srcColorBlendFactor;
		ColorBlendFactor dstColorBlendFactor;
		ColorBlendOperation colorBlendOp;
		ColorBlendFactor srcAlphaBlendFactor;
		ColorBlendFactor dstAlphaBlendFactor;
		ColorBlendOperation alphaBlendOp;
	};

	struct ColorBlend
	{
		ColorBlendAttachment* pColorBlendAttachments = nullptr;
		uint32_t colorBlendAttachmentCount = 0;
		bool logicOpEnable = false;
		float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // Optional
	};

	struct StencilAttachment
	{
		StencilOperation failOp;
		StencilOperation passOp;
		StencilOperation depthFailOp;
		CompareOperation compareOp;
		uint32_t compareMask;
		uint32_t writeMask;
		uint32_t reference;
	};

	struct DepthStencil
	{
		bool enableDepth = false, enableStencil = false;
		CompareOperation depthOpCompare;
		StencilAttachment stencil;
	};

	struct PipelineSpecification
	{
		InputAssembly inputAssembly;
		Viewport viewport;
		Rasterizer rasterizer;
		MultiSampling multisampling;
		ColorBlend colorBlend;
		DepthStencil depthstencil;
	};

	enum class ImageLayoutOption
	{
		None = 0,
		TextureImages,
		FrameBufferImages,
		CubemapImages
	};

	struct DescriptorImageLayoutIncludes
	{
		uint32_t setIndex;
		ImageLayoutOption imageLayoutOption;
	};

	struct PipelineLayoutInfo
	{
		VertexLayoutLinkInfo vertexLayoutLinkInfo;
		uint32_t bindingDescription;
		BufferLayout* pBufferLayouts;
		uint32_t bufferLayoutsCount;
		DescriptorImageLayoutIncludes* pLayoutIncludes = nullptr;
		uint32_t layoutIncludeCount = 0;
		PushConstantRange* pPushConstants;
		uint32_t pushConstantCount;
		RenderPass* renderPass = nullptr;
		PipelineSpecification* pipelineSpecification;
	};

	// Set a Vulkan pipeline specification that can be reused for multiple pipelines
	void setStandardPipelineSpecification(const PipelineSpecification& pipelineSpecification);

	// Get the standard Vulkan pipeline specification that can be reused for multiple pipelines
	PipelineSpecification getStandardPipelineSpecification();
}

#endif