#version 460

layout (location = 0) in vec3 inTexCoords;

layout (location = 0) out vec4 outFragColor;
layout (location = 1) out vec4 outFragColor2;

layout (set = 1, binding = 0) uniform samplerCube samplerCubeMap;

void main() 
{
	outFragColor = texture(samplerCubeMap, inTexCoords);
    outFragColor2 = outFragColor;
}