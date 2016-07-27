#ifndef _CUBE_H_
#define _CUBE_H_

#include "Renderable.h"

#include <GLFW/glfw3.h>

class Cube : public Renderable
{
public:
	Texture textureTop;
	std::string topTexName;
	float texRepeatX;
	float texRepeatY;

	glm::vec3 positionOffset;

	float rotationAngle;

	Cube(glm::vec4 color, glm::vec3 translateVector, glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY, glm::vec3 rotateAxisX, glm::vec3 rotateAxisY, std::string texName, std::string topTexName, float texRepeatX, float texRepeatY);
	virtual ~Cube();

	void init();
	void render(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();
};

#endif