#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexUV;
layout(location = 3) in float texIndex;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexUV;
layout(location = 2) out flat int fragTexIndex;

layout (std140, set = 0, binding = 2) uniform ObjectBuffer 
{
	mat4 matrix;
} ubo;

void main()
{
    gl_Position = ubo.matrix * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
    fragTexUV = inTexUV;
    fragTexIndex = int(texIndex);
}