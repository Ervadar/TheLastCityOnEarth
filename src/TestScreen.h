#ifndef _TESTSCREEN_H_
#define _TESTSCREEN_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Screen.h"
#include "Shaders.h"

class TestScreen : public Screen
{
public:
	float fPyramid[36];
	float fPyramidColor[36];

	Shader vertexShader, fragmentShader; 
	ShaderProgram shaderProgram; 

	GLuint uiVBO[2];
	GLuint uiVAO[1];

	float rotationAngle;

	TestScreen(GLFWwindow * w): Screen(w) { }

	void init();
	void render(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void release();
};

#endif