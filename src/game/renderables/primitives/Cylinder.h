#ifndef _CYLINDER_H_
#define _CYLINDER_H_

#include "Renderable.h"

class Cylinder : public Renderable
{
public:
	Cylinder(glm::vec3 translateVector, glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY, glm::vec3 rotateAxisX, glm::vec3 rotateAxisY, std::string texName);
	virtual ~Cylinder();

	void init();
	void customRender(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();
};

#endif