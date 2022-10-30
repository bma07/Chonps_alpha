#shader vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoord;

uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
	gl_Position = camMatrix * model * vec4(aPos, 1.0);
	TexCoord = aTexCoords;
}



#shader fragment
#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 lightColor;
uniform sampler2D texID;

void main()
{
	FragColor = texture(texID, TexCoord);
}