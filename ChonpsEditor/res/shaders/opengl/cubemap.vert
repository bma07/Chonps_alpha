#version 430 core

layout(location = 0) in vec3 aPos;

out vec3 texCoords;

layout(std140, binding = 1) uniform inUniforms
{
    mat4 camMatrix;
};

void main()
{
	vec4 pos = camMatrix * vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
	texCoords = aPos;
}