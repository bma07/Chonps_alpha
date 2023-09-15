#version 460

const int MAX_TEXTURE_COUNT = 32;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outColor2;

in vec3 fragColor;
in vec2 fragTexCoord;
in flat int fragTexIndex;

uniform sampler2D u_Textures[MAX_TEXTURE_COUNT];

void main()
{
    outColor = texture(u_Textures[fragTexIndex], fragTexCoord);
    //outColor = vec4(fragColor, 1.0f);
}