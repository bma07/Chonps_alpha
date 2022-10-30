#ifndef CHONPS_LIGHTING_H
#define CHONPS_LIGHTING_H

#include "DataTypes.h"

namespace Chonps
{
	struct Light
	{
		vec3 position;
		vec4 color;

		vec3 diffuse;
		vec3 specular;
	};

	struct PointLight : public Light
	{
		float intensity;
	};

	struct DirectLight : public Light
	{
		vec3 direction;
	};

	struct SpotLight : public Light
	{
		vec3 direction;
		
		float innerCone;
		float outerCone;
	};
}

#endif