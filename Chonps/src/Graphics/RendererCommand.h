#ifndef HG_CHONPS_RENDERER_COMMAND_H
#define HG_CHONPS_RENDERER_COMMAND_H

#include "RendererAPI.h"
#include "UniformBuffer.h"

namespace Chonps
{
	void renderInit();

	void renderClear();

	void renderClearColor(const float r, const float g, const float b, const float w = 0.0f);

	void renderDraw(uint32_t vertexCount);

	void renderDrawIndexed(VertexArray* vertexArray);

	void renderDrawIndexed(uint32_t indexCount);
	
	void renderDrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);

	void renderDrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);

	void renderSetStencilReference(uint32_t reference);

	void renderSetStencilMask(uint32_t compareMask, uint32_t writeMask);

	void renderBeginNextFrame();

	void renderDrawSubmit();

	void renderPassBegin();

	void renderPassEnd();

	void renderBindBufferSet(Shader* shader, UniformBuffer* buffer, uint32_t setIndex);

	void renderPushConstant(uint32_t size, uint32_t offset, ShaderStage shaderStage, const void* pValues);
}

#endif