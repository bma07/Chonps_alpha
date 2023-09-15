#ifndef HG_CHONPS_RENDERER_BACKENDS_H
#define HG_CHONPS_RENDERER_BACKENDS_H

#include "Core/QueueChain.h"
#include "Pipeline.h"

namespace Chonps
{
	struct OpenGLSpecific
	{
		const char*				textureLayoutUniformName = "u_Textures";
	};

	struct VulkanSpecific
	{
		uint32_t				textureBinding;
		uint32_t				samplerBinding;
		uint32_t				frameBufferBinding;
		uint32_t				cubemapBinding;
		bool					enableValidationLayers = false;
	};

	struct RendererBackends
	{
		bool					enableColorBlend;
		bool					enableDepthTesting;
		bool					enableStencilTesting;
		bool					enableCullFace;
		RenderTopologyType		topologyType;
		RenderCullFaceMode		cullFaceMode;
		RenderCullFrontFace		cullFrontFace;
		CompareOperation		depthOpCompare;
		StencilAttachment		stencil;
		
		QueueChain<uint32_t>	fontIDsQueue = QueueChain<uint32_t>(0x01, 0xff);
		uint32_t				maxFramesInFlight = 3;
		bool					enableGammaCorrection = false;
		float					gammaValue = 2.2f;
		size_t					maxRenderEntities = 0x4000;
		uint32_t				maxTextureBindingSlots = 32;
		uint32_t				maxObjectIDs = 0xffff;	
		uint32_t				maxGuiDrawObjects = 0x4000;
		uint32_t				maxGuiVerticesPerObjectHint = 0x200;
		uint32_t				maxGuiIndicesPerObjectHint = 0x400;
		bool					enableMultiThreading = false;
		OpenGLSpecific			ogls;
		VulkanSpecific			vks;

		void standardInit()
		{
			enableColorBlend = true;
			enableDepthTesting = true;
			enableStencilTesting = false;
			enableCullFace = true;
			cullFaceMode = CullFaceMode::Back;
			cullFrontFace = CullFrontFace::CounterClockwise;
			topologyType = TopologyType::Triangle;
			vks.textureBinding = 0;
			vks.samplerBinding = 1;
			vks.frameBufferBinding = 0;
			vks.cubemapBinding = 0;
		}
	};
	
	void setRendererBackends(RendererBackends renderBackends);
	RendererBackends* getRendererBackends();

	void renderGammaCorrection(bool correct);
	bool renderGetGammaCorrection();
	void renderSetGammaValue(float gamma);
	float renderGetGammaValue();
}

#endif