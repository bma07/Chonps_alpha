#version 460

layout (location = 0) out vec4 outColor;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexUV;
layout(location = 2) in flat int fragTexIndex;

//uniform sampler2D textures[32];

void main()
{
    outColor = fragColor;
}