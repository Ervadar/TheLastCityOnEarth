#ifndef _OPENGLCONTROL_H_
#define _OPENGLCONTROL_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Screen.h"

/*
Class for OpenGL options and GLFW options/context handling
*/
class GameContext
{
private:
	GLFWwindow * window;
	Screen * screen;

	char * windowTitle;

	int FPSCount;
	bool isFullScreen;

	bool pause;

	static void reshapeCallback(GLFWwindow* window, int width, int height);
	void setWindowParameters();

public:
	GameContext() : window(nullptr), screen(nullptr), windowTitle("The last city on Earth"), FPSCount(0), isFullScreen(false) { }

	int init();
	int run();

	void setScreen(Screen * screen);
};

#endif
