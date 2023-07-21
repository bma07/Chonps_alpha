#version 450

const int MAX_UNIFORM_COUNT = 4096;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

struct DrawData
{
	mat4 camMatrix;
};

layout(set = 0, binding = 0) uniform inUniforms
{
    DrawData drawData[MAX_UNIFORM_COUNT];
} ubo;

void main()
{
    gl_Position = vec4(inPos, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}