#ifndef _PARTICLEEMITTER_H_
#define _PARTICLEEMITTER_H_

#include "Renderable.h"
#include "Particle.h"
#include <random>

struct ParticleEmitterData
{
	GLfloat lifeTimeInSeconds;
	GLuint particlesPerSecond;
	GLfloat initialParticleLife;
	glm::vec3 initialParticlePosition;
	GLfloat initialParticleSize;
	GLfloat initialParticleSpeed;
	GLfloat initialParticleWeight;
	glm::vec4 initialParticleColor;
};

class ParticleEmitter
{
private:
	static const int maxParticles = 1000;
	GLuint particleCount = 0;
	GLboolean generateNewParticles = true;
	int lastUsedParticle = 0;
	Particle particles[maxParticles];
	GLfloat particlePositionAndSizeData[maxParticles * 4];
	GLfloat particleColorData[maxParticles * 4];

	ParticleEmitterData emitterData;

	GLuint positionBuffer;
	GLuint colorBuffer;

	GLuint VAO;
	GLuint vertexVBO;

public:
	ParticleEmitter() {};
	virtual ~ParticleEmitter() {};

	void init(ParticleEmitterData emitterData);
	virtual void render(ShaderProgram & shaderProgram);
	void update(float deltaTime, Camera& camera);
	void destroy();

	int findUnusedParticle();

	void activate();
	void deactivate();
	GLboolean isActive() const;
	GLboolean shouldBeDeactivated(float timePassed) const;
	GLuint getParticleCount() const;

	void initParticle(Particle& p);

};

#endif