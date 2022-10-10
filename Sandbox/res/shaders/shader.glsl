#shader vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTex;

out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;

void main()
{
	texCoord = aTex;
	color = aColor;
	gl_Position = camMatrix * vec4(aPos, 1.0);
}



#shader fragment
#version 430 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

uniform sampler2D texUnit;

void main()
{
	FragColor = texture(texUnit, texCoord);
}