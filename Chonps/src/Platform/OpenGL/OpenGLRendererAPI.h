#ifndef HG_CHONPS_OPENGL_RENDERER_API_H
#define HG_CHONPS_OPENGL_RENDERER_API_H

#include "Graphics/RendererAPI.h"

#include "Platform/WindowAPI/glfwAPI/glfwWindowAPI.h"

namespace Chonps
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual ~OpenGLRendererAPI() {}

		virtual void Init() override;
		virtual void ClearColor(const float r, const float g, const float b, const float w) override;
		virtual void Clear() override;

		virtual void Draw(uint32_t vertexCount) override;
		virtual void DrawIndexed(uint32_t indexCount) override;
		virtual void DrawIndexed(VertexArray* vertexArray) override;
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance) override;
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance) override;

		virtual void BeginNextFrame() override;
		virtual void DrawSubmit() override;
		virtual void RenderPassBegin() override;
		virtual void RenderPassEnd() override;

		virtual void PushConstant(uint32_t size, uint32_t offset, ShaderStage shaderStage, const void* pValues) override;
		virtual void BindBufferSet(Shader* shader, UniformBuffer* buffer, uint32_t setIndex) override;

	private:
		RendererBackends* m_RendererBackends;
		uint32_t m_TopologyType;
	};

	void setOglCurrentWindow(GLFWwindow* window);
}

#endif