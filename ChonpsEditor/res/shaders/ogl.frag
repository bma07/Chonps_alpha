#version 430 core

out vec4 FragColor;

in vec3 fragColor;
in vec2 fragTexCoord;

uniform sampler2D inTex;

void main()
{
    FragColor = texture(inTex, fragTexCoord);
}