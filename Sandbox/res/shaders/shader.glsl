#shader vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aNormal;

out vec3 crntPos;
out vec3 color;
out vec2 texCoord;
out vec3 Normal;

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

in vec3 crntPos;
in vec3 color;
in vec2 texCoord;
in vec3 Normal;

uniform sampler2D texUnit;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

struct PointLight
{
	vec3 position;
	vec4 color;
	vec3 diffuse;
	vec3 specular;
	float intensity;
};

vec4 pointLight()
{	
	vec3 lightVec = lightPos - crntPos;

	float dist = length(lightVec);

	float inten = 1.0;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (diffuse * inten + ambient + specular * inten) * lightColor;
}

vec4 direcLight()
{
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (diffuse + ambient + specular) * lightColor;
}

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (diffuse * inten + ambient + specular) * lightColor;
}

uniform samplerCube skybox;

void main()
{
	// vec3 I = normalize(crntPos - camPos);
    // vec3 R = reflect(I, normalize(Normal));
    // FragColor = vec4(texture(skybox, vec3(R.x, R.y, -R.z)).rgb, 1.0);

	FragColor = vec4(vec3(texture(texUnit, texCoord) * vec4(color, 1.0) * pointLight()), 1.0);
}