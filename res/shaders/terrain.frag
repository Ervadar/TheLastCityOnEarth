#version 330

#define MAX_POINT_LIGHTS 70

smooth in vec2 texCoord;
smooth in vec3 normal;
smooth in vec3 positionWorldspace;
smooth in vec3 eyeDirectionCameraspace;
in vec4 fragPositionLightSpace;
in vec3 fragPositionCameraspace;

uniform sampler2D gSampler[3];
uniform vec4 color;

uniform float renderHeight;
uniform float maxTextureU;
uniform float maxTextureV;

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
uniform DirectionalLight sunLight;
uniform int pointLightsNumber;

out vec4 outputColor;
mat3 CalculateDirectionalLight(DirectionalLight directionalLight, vec3 MaterialDiffuseColor, vec3 vertexNormal, vec3 eyeDirectionCameraspace);
mat3 CalculatePointLight(PointLight pointLight, vec3 MaterialDiffuseColor, vec3 vertexNormal, vec3 fragPositionCameraspace, vec3 eyeDirectionCameraspace);
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 sunDirection);

void main()
{
	vec4 texColor = vec4(0.0);
	
	float scale = positionWorldspace.y/renderHeight;

	const float range1 = 0.15f;
	const float range2 = 0.3f;
	const float range3 = 0.65f;
	const float range4 = 0.85f;

	if(scale >= 0.0 && scale <= range1) texColor = texture2D(gSampler[0], texCoord);
	else if(scale <= range2)
	{
		scale -= range1;
		scale /= (range2-range1);
		
		float scale2 = scale;
		scale = 1.0-scale; 

		texColor += texture2D(gSampler[0], texCoord)*scale;
		texColor += texture2D(gSampler[1], texCoord)*scale2;
	}
	else if(scale <= range3)texColor = texture2D(gSampler[1], texCoord);
	else if(scale <= range4)
	{
		scale -= range3;
		scale /= (range4-range3);
		
		float scale2 = scale;
		scale = 1.0-scale; 
		
		texColor += texture2D(gSampler[1], texCoord)*scale;
		texColor += texture2D(gSampler[2], texCoord)*scale2;		
	}
	else texColor = texture2D(gSampler[2], texCoord);

	mat3 lighting = CalculateDirectionalLight(sunLight, texColor.rgb, normal, eyeDirectionCameraspace);

	for(int i = 0; i < pointLightsNumber; i++)
		lighting += CalculatePointLight(pointLights[i], texColor.rgb, normal, fragPositionCameraspace, eyeDirectionCameraspace);
		
	float shadow = CalculateShadow(fragPositionLightSpace, normal, sunLight.directionCameraspace);
	outputColor = color * vec4(
				  lighting[0] +
				  lighting[1] * (1.0 - shadow) + 
				  lighting[2] * (1.0 - shadow),
				  1.0
				  );
}                      