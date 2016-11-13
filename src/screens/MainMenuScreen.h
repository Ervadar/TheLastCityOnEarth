#ifndef _MAINMENUSCREEN_H_
#define _MAINMENUSCREEN_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Screen.h"
#include "Text.h"
#include <map>

/*
Main game class
*/
class MainMenuScreen : public Screen
{
private:
	// Texts
	//Text text;

	int windowWidth, windowHeight;

	// Helper map for handling key-down events for GLFW
	std::map<int, GLboolean> heldKeysMap;

	//ShaderProgram textShaderProgram;

public:

	MainMenuScreen(GLFWwindow * w): Screen(w) { }
	~MainMenuScreen() { };

	void init();
	void render();
	void update(GLfloat deltaTime);
	void release();

	void updateCamera(GLfloat deltaTime);

	// Input
	void checkMenuInputs();
};

#endif