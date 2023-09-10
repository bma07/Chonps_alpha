#include "cepch.h"
#include "DrawList.h"

namespace Chonps
{
	void DrawList::push_back(DrawCommand drawCmd)
	{
		drawCommands.push_back(drawCmd);
		
		std::vector<uint32_t> batchIndices(drawCmd.pIndices, drawCmd.pIndices + drawCmd.indicesCount);
		for (auto& index : batchIndices)
			index += static_cast<uint32_t>(vertexCount);

		vertices.insert(vertices.end(), drawCmd.pVertices, drawCmd.pVertices + drawCmd.vertexCount * drawCmd.vertexStride);
		indices.insert(indices.end(), batchIndices.begin(), batchIndices.end());

		vertexCount += drawCmd.vertexCount;
		indexCount += drawCmd.indicesCount;
		verticesSize += drawCmd.vertexCount * sizeof(float);
		indicesSize += drawCmd.indicesCount * sizeof(uint32_t);
	}

	void DrawList::clear()
	{
		drawCommands.clear();
		vertices.clear();
		indices.clear();
		indexCount = 0;
		vertexCount = 0;
		indicesSize = 0;
		verticesSize = 0;
	}

	bool DrawList::empty()
	{
		if (indexCount == 0 && vertexCount == 0 &&
			indicesSize == 0 && verticesSize == 0 &&
			vertices.empty() && indices.empty() && drawCommands.empty())
		{
			return true;
		}
		else return false;
	}
}