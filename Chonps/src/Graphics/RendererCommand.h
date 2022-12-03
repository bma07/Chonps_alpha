#ifndef CHONPS_RENDERER_COMMAND_H
#define CHONPS_RENDERER_COMMAND_H

#include "RendererAPI.h"

namespace Chonps
{
	class RendererCommand
	{
	public:
		static void Init();

		static void Clear();
		static void ClearColor(const float r, const float g, const float b, const float w = 0.0f);

		static void FrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height);

		static void GammaCorrection(bool correct);

		static void renderEnableCullFace();
		static void renderDisableCullFace();
	};

	void renderInit();

	void renderClear();
	void renderClearColor(const float r, const float g, const float b, const float w = 0.0f);

	void renderFrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height);

	void renderGammaCorrection(bool correct);

	void renderEnableCullFace();
	void renderDisableCullFace();
}

#endif