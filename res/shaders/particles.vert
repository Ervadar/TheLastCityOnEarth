#version 330

layout(location = 0) in vec3 inPosition;

uniform mat4 M;
uniform mat4 VP;
uniform vec3 cameraUpWorldspace;
uniform vec3 cameraRightWorldspace;

void main()
{
	vec3 particleEffectPositionWorldspace = M[3].xyz;
	vec3 vertexPositionWorldspace = particleEffectPositionWorldspace 
						+ inPosition.y * cameraUpWorldspace
						+ inPosition.x * cameraRightWorldspace;
	gl_Position = VP * vec4(vertexPositionWorldspace, 1.0);
}