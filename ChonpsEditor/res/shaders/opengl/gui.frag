#version 460

out vec4 outColor;

in vec4 fragColor;
in vec2 fragTexUV;
in flat int fragTexIndex;

uniform sampler2D textures;

void main()
{
    outColor = fragColor;
}