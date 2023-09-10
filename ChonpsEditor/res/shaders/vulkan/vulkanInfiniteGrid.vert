#version 460

layout(location = 0) in vec3 inPos;

layout(location = 0) out vec3 nearPoint;
layout(location = 1) out vec3 farPoint;
layout(location = 2) out mat4 camMat;

layout(set = 0, binding = 0) uniform ViewUniforms
{
    mat4 matrix;
} view;

vec3 UnprojectPoint(float x, float y, float z, mat4 camMat) 
{
    vec4 unprojectedPoint = inverse(camMat) * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    nearPoint = UnprojectPoint(inPos.x, inPos.y, 0.0, view.matrix).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(inPos.x, inPos.y, 1.0, view.matrix).xyz; // unprojecting on the far plane
	camMat = view.matrix;
    gl_Position = vec4(inPos, 1.0); // using directly the clipped coordinates
}