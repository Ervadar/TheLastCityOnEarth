#version 330
in vec2 texCoords;
out vec4 color;

uniform sampler2D depthMap;

void main()
{ 
	float depthValue = texture(depthMap, texCoords).r;
    color = vec4(vec3(depthValue), 1.0);
}