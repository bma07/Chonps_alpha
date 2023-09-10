#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

out vec3 fragColor;
out vec2 fragTexCoord;
out flat int fragTexIndex;

struct ObjectData
{
	mat4 camMatrix;
    int texIndex;
};

layout(std140, binding = 0) buffer inUniforms
{
	ObjectData objects[];
};

void main()
{
    gl_Position = objects[gl_InstanceID].camMatrix * vec4(inPos, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragTexIndex = objects[gl_InstanceID].texIndex;
}