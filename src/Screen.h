#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "Camera.h"
#include "Shaders.h"
#include "Renderable.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <functional>
#include <memory>

class Renderable;

/*
Base class for different screens (for example. Menu Screen, Game Screen)
*/
class Screen
{
public:

	GLFWwindow * window;
	int FPSCount;

	Screen(GLFWwindow * w): window(w), FPSCount(0) { }
	virtual ~Screen() { }

	virtual void init() = 0;
	virtual void render() = 0;
	virtual void update(GLfloat deltaTime) = 0;
	virtual void release() = 0;

};

#endif