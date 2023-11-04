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