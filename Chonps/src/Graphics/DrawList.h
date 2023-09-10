#ifndef HG_CHONPS_DRAW_LIST_H
#define HG_CHONPS_DRAW_LIST_H

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

namespace Chonps
{
	struct DrawCommand
	{
		float* pVertices;
		uint32_t vertexCount;
		uint32_t vertexStride;
		uint32_t* pIndices;
		uint32_t indicesCount;
	};

	struct DrawList
	{
		std::vector<DrawCommand> drawCommands;
		size_t indexCount = 0;
		size_t vertexCount = 0;
		size_t indicesSize = 0;
		size_t verticesSize = 0;

		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		void push_back(DrawCommand drawCmd);
		void clear();
		bool empty();
	};
}

#endif