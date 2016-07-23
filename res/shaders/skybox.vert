#version 330

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 normalMatrix;
uniform mat4 lightSpaceMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;

out vec2 texCoord;
out vec3 eyeDirectionCameraspace;
smooth out vec3 normal;
out vec4 fragPositionLightSpace;

void main()
{
	gl_Position = MVP * vec4(inPosition, 1.0);
	vec3 fragPosition = vec3(M * vec4(inPosition, 1.0));
	vec3 inPositionCameraspace = (V * M * vec4(inPosition, 1)).xyz;
	eyeDirectionCameraspace = vec3(0, 0, 0) - inPositionCameraspace;
	texCoord = inTexCoord;
	normal = (normalMatrix * vec4(inNormal, 0.0)).xyz;
	fragPositionLightSpace = lightSpaceMatrix * vec4 (fragPosition, 1.0);
}