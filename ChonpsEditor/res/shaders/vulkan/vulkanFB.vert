#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

layout(std140, set = 0, binding = 0) uniform ObjectBuffer
{
    mat4 matrix;
} ubo;

void main()
{
    gl_Position = vec4(inPos, 0.0f, 1.0f);
    fragTexCoord = inTexCoord;
}