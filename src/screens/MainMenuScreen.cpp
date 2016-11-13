#include "MainMenuScreen.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void MainMenuScreen::init()
{
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	//text.init();

	//textShaderProgram.loadShaderProgram("text.vert", "text.frag");
}

void MainMenuScreen::release()
{
}

void MainMenuScreen::render()
{
	// TEXT
	/*textShaderProgram.useProgram();
	textShaderProgram.setUniform("textureSampler", 0);
	textShaderProgram.setUniform("windowHeight", windowHeight);
	textShaderProgram.setUniform("windowWidth", windowWidth);
	text.render(0, windowHeight, 16);*/
}

void MainMenuScreen::update(GLfloat deltaTime)
{
	checkMenuInputs();

	/*text.setTextValue(
		"F1 - toggle instructions/"
		);*/

	updateCamera(deltaTime);
}

void MainMenuScreen::updateCamera(GLfloat deltaTime)
{
}

// INPUT

void MainMenuScreen::checkMenuInputs()
{
}
