#ifndef _PARTICLEEFFECT_H_
#define _PARTICLEEFFECT_H_

#include "Renderable.h"

struct Particle
{
	glm::vec3 position = glm::vec3(0, 0, -20.0f);
	glm::vec3 speed = glm::vec3(0, 20.0f, 0);
	GLubyte r, g, b, a;
	GLfloat size = 10.0f;
	GLfloat angle, weight;
	GLfloat life = -1.0f;
	GLfloat distanceFromCamera = -1.0f;

	GLboolean isAlive() const
	{
		return (life > 0.0f) ? true : false;
	}
};

class ParticleEffect : public Renderable
{
private:
	static const int maxParticles = 1000;
	GLuint particleCount = 0;
	GLuint particlesPerSecond = 1000;
	GLfloat initialParticleLife = 6.0f;
	GLfloat lifeDecreasingPerSecond = 1.0f;
	int lastUsedParticle = 0;

	Particle particles[maxParticles];
	GLfloat particlePositionAndSizeData[maxParticles * 4];
	GLubyte particleColorData[maxParticles * 4];

	GLuint positionBuffer;
	GLuint colorBuffer;

public:
	ParticleEffect();
	virtual ~ParticleEffect();

	void init();
	virtual void customRender(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();

	int findUnusedParticle();
	void updateAllParticleCameraDistances(Camera& camera);
};

#endif