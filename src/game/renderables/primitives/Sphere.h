#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Renderable.h"

class Sphere : public Renderable
{
public:
	float sphereSpeed;
	GLfloat radius;

	Sphere(glm::vec4 color, glm::vec3 translateVector, GLfloat radius, GLfloat rotateAngleX, GLfloat rotateAngleY, std::string texName);
	virtual ~Sphere();

	void init(Screen * screen);
	void render(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();
};

#endif