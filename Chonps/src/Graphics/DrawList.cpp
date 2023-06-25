#include "cepch.h"
#include "DrawList.h"

namespace Chonps
{
	void DrawList::clear()
	{
		drawCommands.clear();
		vertices.clear();
		indices.clear();
		indexCount = 0;
		vertexCount = 0;
		indexSize = 0;
		vertexSize = 0;
	}
}