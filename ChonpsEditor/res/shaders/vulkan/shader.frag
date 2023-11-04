#version 460

const int MAX_TEXTURE_COUNT = 32;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outColor2;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat int fragTexIndex;

layout(set = 1, binding = 0) uniform texture2D[MAX_TEXTURE_COUNT] textures;
layout(set = 1, binding = 1) uniform sampler samp;

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange(vec2 texUV)
{
    vec2 unitRange = vec2(8.0 / 512.0);
    vec2 screenTexSize = vec2(1.0)/fwidth(texUV);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}


void main()
{
    //outColor = vec4(fragColor, 1.0f);
    //vec3 msd = texture(sampler2D(textures[fragTexIndex], samp), fragTexCoord).rgb;
    //float sd = median(msd.r, msd.g, msd.b);
    //float screenPxDistance = screenPxRange(fragTexCoord) * (sd - 0.5);
    //float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    //outColor = vec4(fragColor, 1.0) * opacity;

    outColor = vec4(fragColor, 1.0) * texture(sampler2D(textures[0], samp), fragTexCoord);
}