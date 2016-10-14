#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct Particle
{
	glm::vec3 position;
	GLfloat speed;
	glm::vec3 direction;
	glm::vec4 color;
	GLfloat size;
	GLfloat angle = 0.0f;
	GLfloat angleX = 0.0f;
	GLfloat angleY = 0.0f;
	GLfloat angleZ = 0.0f;
	glm::vec3 rotationAxisX = glm::vec3(1, 0, 0);
	GLfloat weight;
	GLfloat life = -1.0f;
	GLfloat initialLife;
	GLfloat distanceFromCamera = -1.0f;

	GLboolean isAlive() const
	{
		return (life > 0.0f) ? true : false;
	}
};
#endif