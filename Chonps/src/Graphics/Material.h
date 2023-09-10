#ifndef	HG_CHONPS_MATERIAL_H
#define HG_CHONPS_MATERIAL_H

#include "Shader.h"
#include "Texture.h"

namespace Chonps
{
	struct Material
	{
		float albedof;
		float diffusef;
		float specularf;
		float metallicf;
		float roughnessf;
		float emissionf;
		
		Texture* albedo = nullptr;
		Texture* diffuse = nullptr;
		Texture* specular = nullptr;
		Texture* metallic = nullptr;
		Texture* roughness = nullptr;
		Texture* emission = nullptr;
		Texture* metallicRoughness = nullptr;
		Texture* ambientOcclusion = nullptr;
		Texture* normal = nullptr;

		float transparency;
		float tiling;

		Shader* shader;
	};
}

#endif