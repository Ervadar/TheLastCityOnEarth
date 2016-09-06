#include "ParticleEffect.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "json.hpp"

#include <fstream>

void ParticleEffect::init(std::string effectFilePath)
{
	color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

	inUse = false;
	this->Renderable::init(glm::vec3(0.0f), glm::vec3(10.0f));

	loadEffectFromFile(effectFilePath);
}

void ParticleEffect::customRender(ShaderProgram & shaderProgram)
{
	for (ParticleEmitter& emitter : emitters)
	{
		emitter.render(shaderProgram);
	}
}

void ParticleEffect::update(float deltaTime, Camera& camera)
{
	for (ParticleEmitter& emitter : emitters)
	{
		emitter.update(deltaTime, camera);
		if (emitter.shouldBeDeactivated(timePassed))
		{
			emitter.deactivate();
		}
	}
	timePassed += deltaTime;
	if (!isAnyEmitterActive() && getTotalParticleCount() == 0)
	{
		inUse = false;
		timePassed = 0.0f;
	}
}

void ParticleEffect::destroy()
{
}

void ParticleEffect::activate()
{
	for (ParticleEmitter& emitter : emitters)
	{
		emitter.activate();
	}
}

GLuint ParticleEffect::getTotalParticleCount()
{
	GLuint totalParticleCount = 0;
	for (ParticleEmitter& emitter : emitters)
	{
		totalParticleCount += emitter.getParticleCount();
	}
	return totalParticleCount;
}

GLboolean ParticleEffect::isAnyEmitterActive()
{
	for (ParticleEmitter& emitter : emitters)
	{
		if (emitter.isActive())
		{
			return true;
		}
	}
	return false;
}

void ParticleEffect::loadEffectFromFile(std::string effectFilePath)
{
	std::ifstream effectFile(effectFilePath, std::ifstream::binary);

	// For each line in the file create an emitter
	while (!effectFile.eof())
	{
		ParticleEmitterData data;
		effectFile >> data.lifeTimeInSeconds;
		effectFile >> data.particlesPerSecond;
		effectFile >> data.initialParticleLife;
		effectFile >> data.initialParticlePosition.x;
		effectFile >> data.initialParticlePosition.y;
		effectFile >> data.initialParticlePosition.z;
		effectFile >> data.initialParticleSize;
		effectFile >> data.initialParticleSpeed;
		effectFile >> data.initialParticleWeight;
		effectFile >> data.initialParticleColor.x;
		effectFile >> data.initialParticleColor.y;
		effectFile >> data.initialParticleColor.z;
		effectFile >> data.initialParticleColor.a;

		ParticleEmitter emitter;
		emitter.init(data);
		emitters.push_back(emitter);
	}
	effectFile.close();
}
