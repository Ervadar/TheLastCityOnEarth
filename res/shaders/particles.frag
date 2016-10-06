#version 330

in vec4 particleColor;
in vec2 UV;
uniform sampler2D particleTexture;
out vec4 outputColor;

void main()
{
	vec4 textureColor = texture(particleTexture, UV).rgba;
	outputColor = particleColor * textureColor;
}