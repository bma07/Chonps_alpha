#version 460

out vec4 outColor;

in vec4 fragColor;
in vec2 fragTexUV;
in flat int fragTexIndex;

uniform sampler2D textures;

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange(vec2 texUV)
{
    vec2 unitRange = vec2(4.0) / vec2(textureSize(textures, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(texUV);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main()
{
    vec3 msd = texture(textures, fragTexUV).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange(fragTexUV) * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    outColor = vec4(1.0, 1.0, 1.0, opacity);

    outColor = fragColor;
}