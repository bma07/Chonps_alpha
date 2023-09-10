#include "cepch.h"
#include "RendererBackends.h"

#include "RendererAPI.h"

namespace Chonps
{
	static RendererBackends s_RendererBackends;

	void setRendererBackends(RendererBackends renderBackends)
	{
		s_RendererBackends = renderBackends;
	}

	RendererBackends* getRendererBackends()
	{
		return &s_RendererBackends;
	}

	void renderGammaCorrection(bool correct)
	{
		s_RendererBackends.enableGammaCorrection = correct;
	}

	bool renderGetGammaCorrection()
	{
		return s_RendererBackends.enableGammaCorrection;
	}

	void renderSetGammaValue(float gamma)
	{
		s_RendererBackends.gammaValue = gamma;
	}

	float renderGetGammaValue()
	{
		return s_RendererBackends.gammaValue;
	}
}