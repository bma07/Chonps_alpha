#ifndef OBJ_MODEL_LOADER_H
#define OBJ_MODEL_LOADER_H

#include "Graphics/Mesh.h"

namespace Chonps
{
	std::vector<Mesh> loadOBJModel(const std::string& filepath);
}

#endif