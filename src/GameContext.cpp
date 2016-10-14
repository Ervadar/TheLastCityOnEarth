#include "GameContext.h"

#include <iostream>
#include "GameScreen.h"

#define INITIAL_WIDTH 800
#define INITIAL_HEIGHT 600

void GameContext::reshapeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int GameContext::init()
{
	pause = false;
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWmonitor * monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode * desktop = glfwGetVideoMode(monitor);

	window = glfwCreateWindow(desktop->width, desktop->height, windowTitle, nullptr, nullptr);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW\n";
		return -1;
	}

	setWindowParameters();

	setScreen(new GameScreen(window));

	return 1;
}

int GameContext::run()
{
	do
	{
		if (!pause){
			static double lastTime = glfwGetTime();
			static double fpsLastTime = 0;
			GLfloat currentTime = (GLfloat)glfwGetTime();
			GLfloat deltaTime = GLfloat(currentTime - lastTime);

			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			screen->update(deltaTime);

			screen->render();

			++FPSCount;

			if (currentTime - fpsLastTime >= 1)
			{
				screen->FPSCount = FPSCount;
				FPSCount = 0;
				fpsLastTime = currentTime;
			}

			lastTime = currentTime;

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	} while (!glfwWindowShouldClose(window));

	// Clear shaders, etc.
	screen->release();
	delete screen;

	return 0;
}    

void GameContext::setScreen(Screen * screen)
{
	if (this->screen)
	{
		this->screen->release();
		delete this->screen;
	}
	this->screen = screen;
	this->screen->init();
}

void GameContext::setWindowParameters()
{
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwSetWindowSizeCallback(window, (GLFWwindowsizefun)reshapeCallback);
	glfwSetCursorPos(window, INITIAL_WIDTH / 2, INITIAL_HEIGHT / 2);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	// Accept fragment if it's closer to the camera than the former one
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	// Disable cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}