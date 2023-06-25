#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform inUniforms
{
    vec2 pos;
    mat4 camMatrix;
} ubo;

void main()
{
    gl_Position = vec4(inPos, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}