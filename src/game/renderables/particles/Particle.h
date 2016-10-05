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
	GLfloat angle, weight;
	GLfloat life = -1.0f;
	GLfloat initialLife;
	GLfloat distanceFromCamera = -1.0f;

	GLboolean isAlive() const
	{
		return (life > 0.0f) ? true : false;
	}
};

class ParticlePool
{
	Particle& findUnusedParticle()
	{

	}
};

#endif