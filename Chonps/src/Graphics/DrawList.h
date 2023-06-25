#ifndef CHONPS_DRAW_LIST_H
#define CHONPS_DRAW_LIST_H

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

namespace Chonps
{
	struct DrawCommand
	{
		VertexArray* vertexArray;
		std::vector<uint32_t> textureIDs;
		uint32_t textureArrayIndex;
		uint32_t uniformBufferIndex;
		uint32_t shaderIndex;
	};

	struct DrawList
	{
		std::vector<DrawCommand> drawCommands;
		size_t indexCount;
		size_t vertexCount;
		size_t indexSize;
		size_t vertexSize;

		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		void clear();
	};
}

#endif