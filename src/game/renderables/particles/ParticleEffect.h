#ifndef _PARTICLEEFFECT_H_
#define _PARTICLEEFFECT_H_

#include "Renderable.h"
#include "ParticleEmitter.h"
#include <random>

class ParticleEffect : public Renderable
{
private:
	std::vector<std::unique_ptr<ParticleEmitter>> emitters;
	float timePassed = 0.0f;

public:
	ParticleEffect() {};
	virtual ~ParticleEffect() {};

	void init(std::string effectFilePath);
	virtual void customRender(ShaderProgram & shaderProgram);
	void update(float deltaTime, Camera& camera);
	void destroy();

	void activate();

	GLuint getTotalParticleCount();
	GLboolean isAnyEmitterActive();
	void loadEffectFromFile(std::string effectFilePath);
};

#endif