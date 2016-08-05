#ifndef _MISSILE_H_
#define _MISSILE_H_

#include "Renderable.h"
#include "PointLight.h"

class Missile : public Renderable
{
public:
	static const GLuint OWNER_ENEMY_SHIP = 0;
	static const GLuint OWNER_CANNON = 1;

	GLuint owner;
	GLint strength;

	PointLight * pointLight;

	Missile(GLuint owner);
	~Missile();

	void init(glm::vec3 position, PointLight* pointLight, GLfloat speed, glm::vec3 directionVector);
	void update(float deltaTime);
	void customRender(ShaderProgram & shaderProgram);
	void destroy();
};

#endif