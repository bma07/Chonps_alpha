#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragTexCoord;


layout(set = 1, binding = 0) uniform sampler2D texSampler[8];

void main()
{
    outColor = texture(texSampler[0], fragTexCoord);
}