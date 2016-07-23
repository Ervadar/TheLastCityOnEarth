#version 330

layout(location = 0) in vec2 vertexPositionInScreenSpace;
layout(location = 1) in vec2 vertexUV;

uniform int windowHeight;
uniform int windowWidth;

out vec2 UV;

void main()
{
	vec2 vertexPositionInHomonenousSpace = vertexPositionInScreenSpace - vec2(windowWidth/2, windowHeight/2);
	vertexPositionInHomonenousSpace /= vec2(windowWidth/2, windowHeight/2);
	gl_Position = vec4(vertexPositionInHomonenousSpace, 0, 1);

	UV = vertexUV;
}