#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexUV;
layout(location = 3) in float texIndex;

out vec4 fragColor;
out vec2 fragTexUV;
out flat int fragTexIndex;

layout(std140, binding = 0) uniform inUniforms
{
	mat4 camMatrix;
};

void main()
{
    gl_Position = camMatrix * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
    fragTexUV = inTexUV;
    fragTexIndex = int(texIndex);
}