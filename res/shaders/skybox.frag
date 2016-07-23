#version 330

in vec2 texCoord;
in vec3 eyeDirectionCameraspace;
smooth in vec3 normal;
in vec4 fragPositionLightSpace;

out vec4 outputColor;

//uniform sampler2D sampler;
uniform vec4 color;

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shininess;
};

struct DirectionalLight
{
	vec3 color;
	vec3 directionCameraspace;
	float ambientIntensity;
};

uniform Material material;
uniform DirectionalLight sunLight;

mat3 CalculateDirectionalLight(DirectionalLight directionalLight, vec3 MaterialDiffuseColor, vec3 vertexNormal, vec3 eyeDirectionCameraspace);

float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 sunDirection);

void main()
{
	// Try * color
	mat3 directionalLight = CalculateDirectionalLight(sunLight, texture2D(material.diffuseMap, texCoord).rgb, normal, eyeDirectionCameraspace);

	// Output color (DISTANCE UNIMPLEMENTED)
	float shadow = CalculateShadow(fragPositionLightSpace, normal, sunLight.directionCameraspace);
	outputColor = color * vec4(
				  directionalLight[0] +
				  directionalLight[1] * (1.0 - shadow) + 
				  directionalLight[2] * (1.0 - shadow),
				  1.0
				  );
}