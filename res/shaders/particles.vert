#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 xyzs;	// single particle position and size
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 vertexUV;
layout(location = 4) in vec3 rotationV1;
layout(location = 5) in vec3 rotationV2;
layout(location = 6) in vec3 rotationV3;

uniform mat4 M;
uniform mat4 VP;
uniform vec3 cameraUpWorldspace;
uniform vec3 cameraRightWorldspace;

out vec4 particleColor;
out vec2 UV;

void main()
{
	mat3 rotationMatrix = mat3(rotationV1, rotationV2, rotationV3);
	vec3 rotatedVertexPos = rotationMatrix * vertexPosition;
	float particleSize = xyzs.w;
	vec3 vertexRelativePosToParticle = + rotatedVertexPos.y * cameraUpWorldspace * particleSize
						+ rotatedVertexPos.x * cameraRightWorldspace * particleSize;
	vec3 particleEffectPositionWorldspace = M[3].xyz;
	vec3 vertexPositionWorldspace = particleEffectPositionWorldspace 
						+ xyzs.xyz
						+ vertexRelativePosToParticle;
	gl_Position = VP * vec4(vertexPositionWorldspace, 1.0);
	particleColor = color;
	UV = vertexUV;
}