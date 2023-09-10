#version 460

const int MAX_TEXTURE_COUNT = 4096;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outColor2;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat int fragTexIndex;

layout(set = 1, binding = 0) uniform texture2D[MAX_TEXTURE_COUNT] textures;
layout(set = 1, binding = 1) uniform sampler samp;

void main()
{
    //outColor = vec4(fragColor, 1.0f);
    outColor = texture(sampler2D(textures[fragTexIndex], samp), fragTexCoord);
}