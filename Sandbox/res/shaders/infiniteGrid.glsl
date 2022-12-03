#shader vertex
#version 430 core

layout(location = 0) in vec3 aPos;

out vec3 nearPoint;
out vec3 farPoint;

uniform mat4 camMatrix;

vec3 UnprojectPoint(float x, float y, float z, mat4 camMat) 
{
    vec4 unprojectedPoint = inverse(camMat) * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
	nearPoint = UnprojectPoint(aPos.x, aPos.y, 0.0, camMatrix).xyz;
	farPoint = UnprojectPoint(aPos.x, aPos.y, 1.0, camMatrix).xyz;

	gl_Position = vec4(aPos, 1.0f);
}

#shader fragment
#version 430 core

out vec4 FragColor;

in vec3 nearPoint;
in vec3 farPoint;

float near = 0.01;
float far = 100.0;

uniform mat4 camMatrix;

vec4 grid(vec3 fragPos3D, float scale) 
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -minimumx / scale && fragPos3D.x < minimumx / scale)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -minimumz / scale && fragPos3D.z < minimumz / scale)
        color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) 
{
    vec4 clip_space_pos = camMatrix * vec4(pos.xyz, 1.0);
    return 0.5 + 0.5 * (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) 
{
    vec4 clip_space_pos = camMatrix * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main()
{
	float t = -nearPoint.y / (farPoint.y - nearPoint.y);
	vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
	gl_FragDepth = computeDepth(fragPos3D);

	float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));
	
	FragColor = grid(fragPos3D, 1) * float(t > 0);
	FragColor.a *= fading;
}