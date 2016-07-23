#version 330

uniform mat4 MVP;

layout (location = 0) in vec3 inPosition;

void main()
{
	gl_Position = MVP*vec4(inPosition, 1.0);
}