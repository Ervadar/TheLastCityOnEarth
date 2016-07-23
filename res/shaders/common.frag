#version 330

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

uniform sampler2D shadowMap;

// Returns mat3(ambient part, diffuse part, specular part)
mat3 CalculateDirectionalLight(DirectionalLight directionalLight, vec3 MaterialDiffuseColor, vec3 vertexNormal, vec3 eyeDirectionCameraspace)
{
	vec3 MaterialAmbientColor = directionalLight.ambientIntensity * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3, 0.3, 0.3);

	// Diffuse 
	vec3 n  = normalize(vertexNormal);
	vec3 l = normalize(directionalLight.directionCameraspace);
	float cosTheta = clamp(dot(n, l), 0, 1);

	// Specular
	vec3 E = normalize(eyeDirectionCameraspace);
	vec3 R = reflect(-l, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);

	return mat3(MaterialAmbientColor,
		MaterialDiffuseColor * directionalLight.color * cosTheta,
		MaterialSpecularColor * directionalLight.color * pow(cosAlpha, 5)
		);
}

mat3 CalculatePointLight(PointLight pointLight, vec3 MaterialDiffuseColor, vec3 vertexNormal, vec3 fragPositionCameraspace, vec3 eyeDirectionCameraspace)
{
	vec3 lightDirectionCameraspace = normalize(pointLight.position - fragPositionCameraspace);
	float diffuse = clamp(dot(normalize(vertexNormal), lightDirectionCameraspace), 0, 1);
	vec3 reflectDirection = reflect(-lightDirectionCameraspace, vertexNormal);
	float specular = pow(clamp(dot(normalize(eyeDirectionCameraspace), reflectDirection), 0, 1), 5);
	float lightDistance = length(pointLight.position - fragPositionCameraspace);
	float attenuation = 1.0f / (pointLight.constant + pointLight.linear * lightDistance + pointLight.quadratic * lightDistance * lightDistance);
	
	return mat3(MaterialDiffuseColor * pointLight.ambient * attenuation,
		MaterialDiffuseColor * diffuse * pointLight.diffuse * attenuation,
		vec3(0.3, 0.3, 0.3) * specular * pointLight.specular * attenuation
	);
}

float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 sunDirection)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5; //	Transform to [0, 1] for depth map comparisions
	float currentDepth = projCoords.z;

	float bias = max(0.01 * (1.0 - dot(normalize(normal), sunDirection)), 0.005); // anti shadow acne

	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	// NON PCF
	//float closestDepth = texture(shadowMap, projCoords.xy).r;
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	if (projCoords.z > 1.0) shadow = 0.0;

	return shadow;
}