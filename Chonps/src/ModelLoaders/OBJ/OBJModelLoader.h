#ifndef HG_CHONPS_OBJ_MODEL_LOADER_H
#define HG_CHONPS_OBJ_MODEL_LOADER_H

#include "Graphics/Mesh.h"

namespace Chonps
{
	std::vector<Mesh> loadOBJModel(const std::string& filepath);
}

#endif