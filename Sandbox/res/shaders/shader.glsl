#shader vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aNormal;

out vec3 color;
out vec2 texCoord;
out vec3 Normal;
out vec3 crntPos;

uniform mat4 camMatrix;
uniform mat4 meshMatrix;

void main()
{
	crntPos = vec3(meshMatrix * vec4(aPos, 1.0));
	texCoord = aTex;
	color = aColor;
	Normal = mat3(transpose(inverse(meshMatrix))) * aNormal;

	gl_Position = camMatrix * vec4(crntPos, 1.0);
}



#shader fragment
#version 430 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 Normal;
in vec3 crntPos;

uniform sampler2D texUnit;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{
	// ambient lighting
	float ambient = 0.20;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0);

	// specular lighting
	float specularLight = 0.50;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0), 8);
	float specular = specAmount * specularLight;

	// outputs final color
	FragColor = vec4(vec3(vec4(color, 1.0) * lightColor * (diffuse + ambient + specular)), 1.0);
	//FragColor = vec4(vec3(texture(texUnit, texCoord) * lightColor * (diffuse + ambient + specular)), 1.0);
}