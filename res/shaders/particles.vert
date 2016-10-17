#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 xyzs;	// single particle position and size
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 vertexUV;
layout(location = 4) in vec4 rotationV1;
layout(location = 5) in vec4 rotationV2;
layout(location = 6) in vec4 rotationV3;
layout(location = 7) in vec4 rotationV4;

uniform mat4 M;
uniform mat4 VP;
uniform vec3 cameraUpWorldspace;
uniform vec3 cameraRightWorldspace;
uniform int rotationType;

out vec4 particleColor;
out vec2 UV;

const int ROTATION_DEFAULT = 1;
const int ROTATION_RANDOM_FACING_CAMERA = 2;
const int ROTATION_INTO_DIRECTION = 3;
const int ROTATION_FACING_UP = 4;

void main()
{
	mat4 rotationMatrix = mat4(rotationV1, rotationV2, rotationV3, rotationV4);
	vec4 rotatedVertexPos = rotationMatrix * vec4(vertexPosition, 1);
	float particleSize = xyzs.w;
	vec3 particleEffectPositionWorldspace = M[3].xyz;
	vec3 vertexPositionWorldspace;

	if(rotationType == ROTATION_RANDOM_FACING_CAMERA)
	{
		vec3 vertexRelativePosToParticle = rotatedVertexPos.y * cameraUpWorldspace * particleSize
							+ rotatedVertexPos.x * cameraRightWorldspace * particleSize;
		vertexPositionWorldspace = particleEffectPositionWorldspace 
							+ xyzs.xyz
							+ vertexRelativePosToParticle;
	}
	else if(rotationType == ROTATION_INTO_DIRECTION ||
		rotationType == ROTATION_FACING_UP)
	{
		vertexPositionWorldspace = particleEffectPositionWorldspace 
							+ xyzs.xyz
							+ rotatedVertexPos.xyz * particleSize;
	}
	gl_Position = VP * vec4(vertexPositionWorldspace, 1.0);
	particleColor = color;
	UV = vertexUV;
}