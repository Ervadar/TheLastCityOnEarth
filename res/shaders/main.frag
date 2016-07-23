#version 330

#define MAX_POINT_LIGHTS 70

in vec2 texCoord;
in vec3 eyeDirectionCameraspace;
smooth in vec3 normal;
in vec4 fragPositionLightSpace;
in vec3 fragPositionCameraspace;

out vec4 outputColor;

//uniform sampler2D sampler;
uniform vec4 color;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct DirectionalLight
{
	vec3 color;
	vec3 directionCameraspace;
	float ambientIntensity;
};

struct PointLight
{
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform Material material;
uniform DirectionalLight sunLight;

uniform int pointLightsNumber;

mat3 CalculateDirectionalLight(DirectionalLight directionalLight, vec3 MaterialDiffuseColor, vec3 vertexNormal, vec3 eyeDirectionCameraspace);
mat3 CalculatePointLight(PointLight pointLight, vec3 MaterialDiffuseColor, vec3 vertexNormal, vec3 fragPositionCameraspace, vec3 eyeDirectionCameraspace);
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 sunDirection);

void main()
{
	mat3 lighting = CalculateDirectionalLight(sunLight, texture(material.texture_diffuse1, texCoord).rgb, normal, eyeDirectionCameraspace);

	for(int i = 0; i < pointLightsNumber; i++)
		lighting += CalculatePointLight(pointLights[i], texture(material.texture_diffuse1, texCoord).rgb, normal, fragPositionCameraspace, eyeDirectionCameraspace);
		
	// Output color (DISTANCE UNIMPLEMENTED)
	float shadow = CalculateShadow(fragPositionLightSpace, normal, sunLight.directionCameraspace);
	outputColor = color * vec4(
				  lighting[0] +
				  lighting[1] * (1.0 - shadow) + 
				  lighting[2] * (1.0 - shadow),
				  1.0
				  );
}