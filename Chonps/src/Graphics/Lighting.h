#ifndef LIGHTING_H
#define LIGHTING_H

#include "DataTypes.h"

namespace Chonps
{
	struct Light
	{
		Light(vec3 pos, vec4 color)
			: position(pos), color(color) {}

		vec3 position;
		vec4 color;
		vec3  direction;
		float cutOff;
	};

	struct DirectLight : public Light
	{
		vec3 position;
		vec4 color;
		vec3 direction;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	};

	struct PointLight : public Light
	{
		vec3 position;
		vec4 color;

		float constant;
		float linear;
		float quadratic;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	};

	struct SpotLight : public Light
	{
		vec3 position;
		vec4 color;
		vec3  direction;
		
		float innerCone;
		float outerCone;
	};
}

#endif