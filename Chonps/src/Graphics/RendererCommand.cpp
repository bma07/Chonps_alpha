#include "cepch.h"
#include "RendererCommand.h"

#include "RendererBackends.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Chonps
{
	static RendererAPI* s_RendererAPIAddressRenderCommand = nullptr;

	// Sets and init specific render function calls before drawing vertices
	void renderInit()
	{
		static bool s_Init = false;
		s_RendererAPIAddressRenderCommand = getRendererAPI();

		if (!s_Init)
		{
			getRendererAPI()->Init();
			s_Init = true;
		}
		else CHONPS_CORE_WARN("WARNING: Attempting to call renderInit more than once! renderInit must be called only one time to initialize rendering.");
	}

	// Clear function clears the window viewport with all of the vertices drawn on it
	void renderClear()
	{
		s_RendererAPIAddressRenderCommand->Clear();
	}

	// Clears the window and set the color of the viewport/background
	void renderClearColor(const float r, const float g, const float b, const float w /*= 0.0f*/)
	{
		s_RendererAPIAddressRenderCommand->ClearColor(r, g, b, w);
	}

	// Prepares any data related to rendering before the rendering begins, 
	// this function is usually called before the main render loop and after
	// all vertex, index, texture data, ect. have been created.
	void renderPrepareDraw()
	{
		GraphicsAPI GraphicsAPI = getGraphicsAPI();

		if (GraphicsAPI == GraphicsAPI::Vulkan)
			vks::vkImplPrepareDraw();
	}

	void renderDrawIndexed(VertexArray* vertexArray)
	{
		s_RendererAPIAddressRenderCommand->DrawIndexed(vertexArray);
	}

	void renderDrawIndexed(uint32_t indexCount)
	{
		s_RendererAPIAddressRenderCommand->DrawIndexed(indexCount);
	}

	void renderDrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance)
	{
		s_RendererAPIAddressRenderCommand->DrawInstanced(vertexCount, instanceCount, firstInstance);
	}

	void renderDrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
	{
		s_RendererAPIAddressRenderCommand->DrawIndexedInstanced(indexCount, instanceCount, firstInstance);
	}

	void renderDraw(uint32_t vertexCount)
	{
		s_RendererAPIAddressRenderCommand->Draw(vertexCount);
	}

	void renderBeginNextFrame()
	{
		s_RendererAPIAddressRenderCommand->BeginNextFrame();
	}

	void renderDrawSubmit()
	{
		s_RendererAPIAddressRenderCommand->DrawSubmit();
	}

	void renderPassBegin()
	{
		s_RendererAPIAddressRenderCommand->RenderPassBegin();
	}

	void renderPassEnd()
	{
		s_RendererAPIAddressRenderCommand->RenderPassEnd();
	}

	void renderBindBufferSet(Shader* shader, UniformBuffer* buffer, uint32_t setIndex)
	{
		s_RendererAPIAddressRenderCommand->BindBufferSet(shader, buffer, setIndex);
	}

	void renderPushConstant(uint32_t size, uint32_t offset, ShaderStage shaderStage, const void* pValues)
	{
		s_RendererAPIAddressRenderCommand->PushConstant(size, offset, shaderStage, pValues);
	}
}