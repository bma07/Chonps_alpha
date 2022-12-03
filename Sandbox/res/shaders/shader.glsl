#shader vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;

out vec3 crntPos;
out vec3 color;
out vec2 texCoord;
out vec3 Normal;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 camMatrix;
uniform mat4 meshMatrix;
uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{
	crntPos = vec3(meshMatrix * vec4(aPos, 1.0));
	texCoord = aTex;
	color = aColor;
	Normal = mat3(transpose(inverse(meshMatrix))) * aNormal;

	vec3 T = normalize(vec3(meshMatrix * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(meshMatrix * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(meshMatrix * vec4(aNormal, 0.0)));
	mat3 TBN = transpose(mat3(T, B, N)); // Inverse of TBN brings to tangent space
	TangentLightPos = TBN * lightPos;
    TangentViewPos = TBN * camPos;
    TangentFragPos = TBN * crntPos;

	gl_Position = camMatrix * vec4(crntPos, 1.0);
}



#shader fragment
#version 430 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 color;
in vec2 texCoord;
in vec3 Normal;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

uniform sampler2D meshTexDiffuse;
uniform sampler2D meshTexSpecular;
uniform sampler2D meshTexNormal;

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
	vec3 lightVec = TangentLightPos - TangentFragPos;

	float dist = length(lightVec);

	float inten = 1.0;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	//vec3 normal = normalize(Normal);
	
    vec3 normal = texture(meshTexNormal, texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
	
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(TangentViewPos - TangentFragPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(meshTexDiffuse, texCoord) * (diffuse * inten + ambient) + texture(meshTexSpecular, texCoord).r * specular * inten) * lightColor;
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

	return (texture(meshTexDiffuse, texCoord) * (diffuse + ambient) + texture(meshTexSpecular, texCoord).r * specular) * lightColor;
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

	return (texture(meshTexDiffuse, texCoord) * (diffuse * inten + ambient) + texture(meshTexSpecular, texCoord).r * specular * inten) * lightColor;
}

uniform samplerCube skybox;

void main()
{
	// vec3 I = normalize(crntPos - camPos);
    // vec3 R = reflect(I, normalize(Normal));
    // FragColor = vec4(texture(skybox, vec3(R.x, R.y, -R.z)).rgb, 1.0);
	
	//FragColor = vec4(vec3(color), 1.0);
	FragColor = vec4(vec3(pointLight()) * color, 1.0);
}