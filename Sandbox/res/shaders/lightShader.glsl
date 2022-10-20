#shader vertex
#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
	gl_Position = camMatrix * model * vec4(aPos, 1.0);
}



#shader fragment
#version 430 core

out vec4 FragColor;

uniform vec4 lightColor;

void main()
{
	FragColor = lightColor;
}