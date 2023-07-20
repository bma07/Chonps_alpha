#version 460

const int MAX_TEXTURE_COUNT = 4096;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(set = 1, binding = 0) uniform texture2D[MAX_TEXTURE_COUNT] textures;
layout(set = 1, binding = 1) uniform sampler samp;

layout(push_constant) uniform constants
{
	int texIndex[32];
} pc;

void main()
{
    outColor = texture(sampler2D(textures[pc.texIndex[0]], samp), fragTexCoord);
}