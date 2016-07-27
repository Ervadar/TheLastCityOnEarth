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

public:
	bool pause;

	GameContext(): window(nullptr), screen(nullptr), windowTitle("The last city on Earth"), FPSCount(0), isFullScreen(false) { }

	static void reshapeCallback(GLFWwindow* window, int width, int height);

	int init();
	int run();
	void setScreen(Screen * screen);

	void setWindowParameters();
};

#endif
