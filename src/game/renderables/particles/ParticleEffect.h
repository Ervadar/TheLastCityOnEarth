#ifndef _PARTICLEEFFECT_H_
#define _PARTICLEEFFECT_H_

#include "Renderable.h"
#include "ParticleEmitter.h"
#include <random>

class ParticleEffect : public Renderable
{
private:
	static std::map<std::string, std::vector<ParticleEmitterData>> loadedEmitters;
	std::vector<std::unique_ptr<ParticleEmitter>> emitters;
	float timePassed = 0.0f;

public:
	ParticleEffect(std::string effectName, float scale);
	virtual ~ParticleEffect() {};

	void init(std::string effectName, float scale);
	virtual void customRender(ShaderProgram & shaderProgram);
	void update(float deltaTime, Camera& camera);
	void destroy();

	void activate();

	GLuint getTotalParticleCount();
	GLboolean isAnyEmitterActive();
	static void loadEffectFromFile(std::string effectName);
};

#endif