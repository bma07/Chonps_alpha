#shader vertex
#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
}


#shader fragment
#version 430 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    float gamma = 2.2f;
    FragColor = vec4(vec3(pow(texture(screenTexture, TexCoords).rgb, vec3(1.0f / gamma))), 1.0);
}