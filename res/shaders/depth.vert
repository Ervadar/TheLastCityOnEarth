#version 330

layout(location = 0) in vec3 inPosition;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

void main()
{
	gl_Position = MVP * vec4(inPosition, 1.0f);
}