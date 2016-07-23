#version 330

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 normalMatrix;
uniform mat4 lightSpaceMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;

smooth out vec2 texCoord;
smooth out vec3 normal;
smooth out vec3 positionWorldspace;
smooth out vec3 eyeDirectionCameraspace;
out vec4 fragPositionLightSpace;
out vec3 fragPositionCameraspace;

uniform mat4 heightmapScaleMatrix;

void main()
{
  vec4 inPositionScaled = heightmapScaleMatrix*vec4(inPosition, 1.0);
   
  gl_Position = MVP*inPositionScaled;

  vec3 inPositionCameraspace = (V * M * inPositionScaled).xyz;
  eyeDirectionCameraspace = vec3(0, 0, 0) - inPositionCameraspace;
  
  texCoord = inTexCoord;
  normal = (normalMatrix * vec4(inNormal, 0.0)).xyz;
   
  positionWorldspace = (M * inPositionScaled).xyz;
  fragPositionLightSpace = lightSpaceMatrix * inPositionScaled;
  fragPositionCameraspace = vec3(V * M * inPositionScaled);
}