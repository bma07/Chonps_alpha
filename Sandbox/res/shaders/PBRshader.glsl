#shader vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Color;
out vec2 TexCoords;
out vec3 Normal;
out vec3 LightPos;
out vec3 CamPos;

uniform mat4 camMatrix;
uniform mat4 meshMatrix;
uniform vec3 lightPos;
uniform vec3 camPos;



void main()
{
	FragPos = vec3(meshMatrix * vec4(aPos, 1.0));
	TexCoords = aTex;
	Color = aColor;
	Normal = mat3(transpose(inverse(meshMatrix))) * aNormal;

	vec3 T = normalize(vec3(meshMatrix * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(meshMatrix * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(meshMatrix * vec4(aNormal, 0.0)));
	mat3 TBN = transpose(mat3(T, B, N)); // Inverse of TBN brings to tangent space
	LightPos = TBN * lightPos;
    CamPos = TBN * camPos;
    FragPos = TBN * FragPos;

	gl_Position = camMatrix * vec4(vec3(meshMatrix * vec4(aPos, 1.0)), 1.0);
}

#shader fragment
#version 430 core

