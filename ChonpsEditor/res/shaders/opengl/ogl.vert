#version 430 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

out vec3 fragColor;

void main()
{
    gl_Position = vec4(inPos, 1.0);
    fragColor = inColor;
}