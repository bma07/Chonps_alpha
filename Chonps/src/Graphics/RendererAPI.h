#ifndef HG_CHONPS_RENDERER_API_H
#define HG_CHONPS_RENDERER_API_H

#include "Camera.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "VertexArray.h"
#include "RendererBackends.h"

namespace Chonps
{
	enum class GraphicsAPI
	{
		None = 0,
		OpenGL,
		Vulkan,
		DirectX
	};

	bool setRenderContext(GraphicsAPI api);

	GraphicsAPI getGraphicsAPI();
	std::string getGraphicsAPIName();


	class RendererAPI
	{
	public:
		virtual ~RendererAPI() {}
		virtual void Init() = 0;
		virtual void ClearColor(const float r, const float g, const float b, const float w) = 0;
		virtual void Clear() = 0;

		virtual void Draw(uint32_t vertexCount) = 0;
		virtual void DrawIndexed(uint32_t indexCount) = 0;
		virtual void DrawIndexed(VertexArray* vertexArray) = 0;
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance) = 0;
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance) = 0;

		virtual void BeginNextFrame() = 0;
		virtual void DrawSubmit() = 0;
		virtual void RenderPassBegin() = 0;
		virtual void RenderPassEnd() = 0;

		virtual void BindBufferSet(Shader* shader, UniformBuffer* buffer, uint32_t setIndex) = 0;
		virtual void PushConstant(uint32_t size, uint32_t offset, ShaderStage shaderStage, const void* pValues) = 0;

	};

	RendererAPI* getRendererAPI();
	bool createRendererAPI(RendererBackends* rendererBackends);
	void destroyRendererAPI();

	void textureBinding(uint32_t textureBinding, uint32_t samplerBinding, uint32_t frameBufferBinding, uint32_t cubemapBinding);
}

#endif