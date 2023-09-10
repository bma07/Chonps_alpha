#include "cepch.h"
#include "BatchRenderer.h"

#include "RendererBackends.h"
#include "RendererCommand.h"

namespace Chonps
{
	BatchRenderer::BatchRenderer(VertexLayout* pLayouts, uint32_t layoutCount, uint32_t vertexBufferSizeAmount, uint32_t indexBufferSizeAmount)
	{
		// Create Vertex Array Buffers
		m_VertexArray = createVertexArray();

		uint32_t vertexBufferSize = vertexBufferSizeAmount;
		m_VertexBuffer = createVertexBuffer(nullptr, vertexBufferSize, BufferState::Dynamic);

		uint32_t indexBufferSize = indexBufferSizeAmount;
		m_IndexBuffer = createIndexBuffer(nullptr, indexBufferSize, BufferState::Dynamic);

		VertexLayoutLinkInfo linkInfo{};
		linkInfo.pLayouts = pLayouts;
		linkInfo.layoutCount = layoutCount;

		m_VertexArray->LinkBuffers(m_VertexBuffer, m_IndexBuffer, &linkInfo);

		m_VertexArray->Unbind();
		m_VertexBuffer->Unbind();
		m_IndexBuffer->Unbind();
	}

	BatchRenderer::BatchRenderer(BatchCreateInfo* createInfo)
	{
		// Create Vertex Array Buffers
		m_VertexArray = createVertexArray();

		uint32_t vertexBufferSize = createInfo->vertexBufferSize;
		m_VertexBuffer = createVertexBuffer(nullptr, vertexBufferSize, BufferState::Dynamic);

		uint32_t indexBufferSize = createInfo->indexBufferSize;
		m_IndexBuffer = createIndexBuffer(nullptr, indexBufferSize, BufferState::Dynamic);

		VertexLayoutLinkInfo linkInfo{};
		linkInfo.pLayouts = createInfo->pLayouts;
		linkInfo.layoutCount = createInfo->layoutCount;

		m_VertexArray->LinkBuffers(m_VertexBuffer, m_IndexBuffer, &linkInfo);

		m_VertexArray->Unbind();
		m_VertexBuffer->Unbind();
		m_IndexBuffer->Unbind();
	}

	void BatchRenderer::DrawBegin()
	{
		m_DrawList.clear();
	}

	void BatchRenderer::DrawSubmit()
	{
		CHONPS_CORE_ASSERT(!m_DrawList.empty(), "Draw List is empty!");

		m_VertexBuffer->Subdata(m_DrawList.vertices.data(), static_cast<uint32_t>(sizeof(float) * m_DrawList.vertices.size()));
		m_IndexBuffer->Subdata(m_DrawList.indices.data(), static_cast<uint32_t>(sizeof(uint32_t) * m_DrawList.indices.size()));

		m_VertexArray->Bind();
		renderDrawIndexed(m_IndexBuffer->GetCount());
		m_VertexArray->Unbind();

		m_DrawList.clear();
	}

	void BatchRenderer::SubdataVertices(float* vertices, uint32_t size)
	{
		m_VertexBuffer->Subdata(vertices, size);
	}

	void BatchRenderer::SubdataIndices(uint32_t* indices, uint32_t size)
	{
		m_IndexBuffer->Subdata(indices, size);
	}

	void BatchRenderer::PushDrawCommand(DrawCommand drawCmd)
	{
		m_DrawList.push_back(drawCmd);
	}

	void BatchRenderer::Delete()
	{
		m_VertexBuffer->Delete();
		m_IndexBuffer->Delete();
		m_VertexArray->Delete();

		m_DrawList.clear();
	}
}