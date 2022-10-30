#shader vertex
#version 430 core

layout(location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 cubemapProj;
uniform mat4 cubemapView;

void main()
{
	vec4 pos = cubemapProj * cubemapView * vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
	texCoords = vec3(aPos.x, aPos.y, -aPos.z);
}

#shader fragment
#version 430 core

out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{
	float gamma = 2.2f;
	FragColor = vec4(vec3(pow(texture(skybox, texCoords).rgb, vec3(gamma))), 1.0);
}