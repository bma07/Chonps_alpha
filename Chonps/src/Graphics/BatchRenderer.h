#ifndef HG_CHONPS_BATCH_RENDERER_H
#define HG_CHONPS_BATCH_RENDERER_H

#include "VertexArray.h"
#include "DrawList.h"

namespace Chonps
{
	struct BatchCreateInfo
	{
		VertexLayout* pLayouts;				// pointer to one or an array of Vertex layouts
		uint32_t layoutCount;				// the number Vertex layouts (pLayouts)
		uint32_t vertexBufferSize;		// the estimated size of the vertex buffer
		uint32_t indexBufferSize;			// the estimated size of the index buffer
	};

	class BatchRenderer
	{
	public:
		BatchRenderer() = default;
		BatchRenderer(VertexLayout* pLayouts,
			uint32_t layoutCount,
			uint32_t vertexBufferSizeAmount,
			uint32_t indexBufferSizeAmount);

		BatchRenderer(BatchCreateInfo* createInfo);
		
		void DrawBegin();
		void DrawSubmit();
		void SubdataVertices(float* vertices, uint32_t size);
		void SubdataIndices(uint32_t* indices, uint32_t size);
		void PushDrawCommand(DrawCommand drawCmd);

		void Delete();

		VertexArray* getVertexArray() { return m_VertexArray; }
		VertexBuffer* getVertexBuffer() { return m_VertexBuffer; }
		IndexBuffer* getIndexBuffer() { return m_IndexBuffer; }
		DrawList& getDrawList() { return m_DrawList; }

		float* getVertices() { return m_VertexBuffer->GetVertices(); }
		uint32_t* getIndices() { return m_IndexBuffer->GetIndices(); }

	private:
		VertexArray* m_VertexArray;
		VertexBuffer* m_VertexBuffer;
		IndexBuffer* m_IndexBuffer;

		DrawList m_DrawList;
	};
}


#endif