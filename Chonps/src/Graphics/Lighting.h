#ifndef HG_CHONPS_LIGHTING_H
#define HG_CHONPS_LIGHTING_H

#include "Core/DataTypes.h"

namespace Chonps
{
	struct Light
	{
		glm::vec3 position;
		glm::vec4 color;

		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct PointLight : public Light
	{
		float intensity;
	};

	struct DirectLight : public Light
	{
		glm::vec3 direction;
	};

	struct SpotLight : public Light
	{
		glm::vec3 direction;
		
		float innerCone;
		float outerCone;
	};
}

#endif