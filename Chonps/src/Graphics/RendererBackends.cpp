#include "cepch.h"
#include "RendererBackends.h"

#include "RendererAPI.h"

namespace Chonps
{
	static RendererBackends s_RendererBackends;
	

	void renderEnableColorBlend(bool enable)
	{
		s_RendererBackends.enableColorBlend = enable;
	}

	void renderGammaCorrection(bool correct)
	{
		s_RendererBackends.enableGammaCorrection = correct;
	}

	bool renderGetGammaCorrection()
	{
		return s_RendererBackends.enableGammaCorrection;
	}

	void renderSetGamma(float gamma)
	{
		s_RendererBackends.gammaValue = gamma;
	}

	float renderGetGamma()
	{
		return s_RendererBackends.gammaValue;
	}

	void renderSetTopologyType(RenderTopologyType type)
	{
		type != RenderTopologyType::Default ? s_RendererBackends.topologyType = type : s_RendererBackends.topologyType = RenderTopologyType::Triangle;
	}

	RenderTopologyType renderGetTopologyType()
	{
		return s_RendererBackends.topologyType;
	}

	void renderEnableCullFace(bool enable)
	{
		s_RendererBackends.enableCullFace = enable;
		if (enable) getRendererAPI()->EnableCullFace();
		else getRendererAPI()->DisableCullFace();
	}

	void renderSetCullFaceMode(RenderCullFaceMode cullFaceMode)
	{
		s_RendererBackends.cullFaceMode = cullFaceMode;
	}

	RenderCullFaceMode renderGetCullFaceMode()
	{
		return s_RendererBackends.cullFaceMode;
	}

	void renderSetCullFrontFace(RenderCullFrontFace cullFrontFace)
	{
		s_RendererBackends.cullFrontFace = cullFrontFace;
	}

	RenderCullFrontFace renderGetCullFrontFace()
	{
		return s_RendererBackends.cullFrontFace;
	}
}