#version 460

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outColor2;

in vec3 fragColor;
in vec2 fragTexCoord;
in flat int fragTexIndex;

uniform sampler2D textures;

void main()
{
    outColor = texture(textures, fragTexCoord);
    //outColor = vec4(fragColor, 1.0f);
}