#ifndef HG_CHONPS_GUI_SHADERS_H
#define HG_CHONPS_GUI_SHADERS_H

// Vulkan Impl

// Gui Shaders
const char* guiVert_vkImpl = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexUV;
layout(location = 3) in float texIndex;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexUV;
layout(location = 2) out flat int fragTexIndex;

layout(push_constant) uniform constants
{
	mat4 matrix;
    float unitRange;
} pc;

void main()
{
    gl_Position = pc.matrix * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
    fragTexUV = inTexUV;
    fragTexIndex = int(texIndex);
}
)";

const char* guiFrag_vkImpl = R"(
#version 460

const int MAX_TEXTURE_COUNT = 32;

layout (location = 0) out vec4 outColor;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexUV;
layout(location = 2) in flat int fragTexIndex;

layout(set = 0, binding = 0) uniform texture2D[MAX_TEXTURE_COUNT] textures;
layout(set = 0, binding = 1) uniform sampler samp;

void main()
{
    outColor = fragColor;
}
)";


// Font Shaders
const char* fontVert_vkImpl = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexUV;
layout(location = 3) in float texIndex;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexUV;
layout(location = 2) out flat int fragTexIndex;
layout(location = 3) out float fragUnitRange;

layout(push_constant) uniform constants
{
	mat4 matrix;
    float unitRange;
} pc;

void main()
{
    gl_Position = pc.matrix * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
    fragTexUV = inTexUV;
    fragTexIndex = int(texIndex);
    fragUnitRange = pc.unitRange;
}
)";

const char* fontFrag_vkImpl = R"(
#version 460

const int MAX_TEXTURE_COUNT = 32;

layout (location = 0) out vec4 outColor;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexUV;
layout(location = 2) in flat int fragTexIndex;
layout(location = 3) in float fragUnitRange;

layout(set = 0, binding = 0) uniform texture2D[MAX_TEXTURE_COUNT] textures;
layout(set = 0, binding = 1) uniform sampler samp;

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange(vec2 texUV)
{
    vec2 unitRange = vec2(fragUnitRange);
    vec2 screenTexSize = vec2(1.0)/fwidth(texUV);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main()
{
    outColor = vec4(fragColor.rgb, fragColor.a * texture(sampler2D(textures[fragTexIndex], samp), fragTexUV).r);
}
)";


// OpenGL Impl
const char* guiVert_oglImpl = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexUV;
layout(location = 3) in float texIndex;

out vec4 fragColor;
out vec2 fragTexUV;
out flat int fragTexIndex;

uniform mat4 u_CamMatrix;

void main()
{
    gl_Position = u_CamMatrix * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
    fragTexUV = inTexUV;
    fragTexIndex = int(texIndex);
}
)";

const char* guiFrag_oglImpl = R"(
#version 460

out vec4 outColor;

in vec4 fragColor;
in vec2 fragTexUV;
in flat int fragTexIndex;


void main()
{
    outColor = fragColor;
}
)";

// Font Shaders
const char* fontVert_oglImpl = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexUV;
layout(location = 3) in float texIndex;

out vec4 fragColor;
out vec2 fragTexUV;
out flat int fragTexIndex;

uniform mat4 u_CamMatrix;

void main()
{
    gl_Position = u_CamMatrix * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
    fragTexUV = inTexUV;
    fragTexIndex = int(texIndex);
}
)";

const char* fontFrag_oglImpl = R"(
#version 460

const int MAX_TEXTURE_COUNT = 32;

out vec4 outColor;

in vec4 fragColor;
in vec2 fragTexUV;

uniform sampler2D u_Textures[MAX_TEXTURE_COUNT];

void main()
{
    outColor = fragColor * vec4(1.0, 1.0, 1.0, texture(u_Textures[0], fragTexUV).r);
}
)";
#endif