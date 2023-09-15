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
		uint32_t albedoSlot = 0;

		Texture* diffuse = nullptr;
		uint32_t diffuseSlot = 0;

		Texture* specular = nullptr;
		uint32_t specularSlot = 0;

		Texture* metallic = nullptr;
		uint32_t metallicSlot = 0;

		Texture* roughness = nullptr;
		uint32_t roughnessSlot = 0;

		Texture* emission = nullptr;
		uint32_t emissionSlot = 0;

		Texture* metallicRoughness = nullptr;
		uint32_t metallicRoughnessSlot = 0;

		Texture* ambientOcclusion = nullptr;
		uint32_t ambientOcclusionSlot = 0;

		Texture* normal = nullptr;
		uint32_t normalSlot = 0;


		float transparency;
		float tiling;
	};
}

#endif