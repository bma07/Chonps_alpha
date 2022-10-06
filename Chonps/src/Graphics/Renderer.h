#ifndef RENDERER_H
#define RENDERER_H

#include "RendererAPI.h"

namespace Chonps
{
	void renderDraw(const size_t& count);

	void renderBeginScene();
	void renderEndScene();

	void renderClear();
	void renderClearColor(const float r, const float g, const float b, const float w = 0.0f);

	void setRendererAPI();
	RendererAPI* getRendererAPI();
}

#endif