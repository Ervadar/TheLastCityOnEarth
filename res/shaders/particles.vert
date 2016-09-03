#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 xyzs;	// single particle position and size
layout(location = 2) in vec4 color;

uniform mat4 M;
uniform mat4 VP;
uniform vec3 cameraUpWorldspace;
uniform vec3 cameraRightWorldspace;

out vec4 particleColor;

void main()
{
	float particleSize = xyzs.w;
	vec3 particleEffectPositionWorldspace = M[3].xyz;
	vec3 vertexPositionWorldspace = particleEffectPositionWorldspace 
						+ xyzs.xyz
						+ vertexPosition.y * cameraUpWorldspace * particleSize
						+ vertexPosition.x * cameraRightWorldspace * particleSize;
	gl_Position = VP * vec4(vertexPositionWorldspace, 1.0);
	particleColor = color;
}