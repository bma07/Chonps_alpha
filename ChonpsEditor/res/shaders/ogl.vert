#version 430 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

out vec3 fragColor;
out vec2 fragTexCoord;

layout(std140, binding = 0) uniform inUniforms
{
    mat4 camMatrix;
};

void main()
{
    gl_Position = camMatrix * vec4(inPos, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}