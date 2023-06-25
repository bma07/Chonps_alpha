#ifndef CHONPS_RENDERER_COMMAND_H
#define CHONPS_RENDERER_COMMAND_H

#include "RendererAPI.h"

namespace Chonps
{
	void renderInit();

	void renderClear();

	void renderClearColor(const float r, const float g, const float b, const float w = 0.0f);

	void renderPrepareDraw();

	void renderBeginNextFrame();

	void renderDrawSubmit();

	void renderFrameBufferBlit(uint32_t readFBO, uint32_t drawFBO, uint32_t width, uint32_t height);
}

#endif