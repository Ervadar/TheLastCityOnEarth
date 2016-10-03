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
	std::ifstream effectFile(effectFilePath);
	nlohmann::json json(effectFile);

	auto& jsonEmitters = json["particleEmitters"];

	for (unsigned int i = 0; i < jsonEmitters.size(); ++i)
	{
		ParticleEmitter emitter;
		ParticleEmitterData data;
		auto& jsonEmitter = jsonEmitters[i];
		data.lifeTimeInSeconds = data.getVarFromJson<GLfloat>(jsonEmitter["lifeTimeInSeconds"]);
		data.particlesPerSecond = data.getVarFromJson<GLuint>(jsonEmitter["particlesPerSecond"]);
		data.initialParticleLife = data.getVarFromJson<GLfloat>(jsonEmitter["particleLife"]);
		data.initialParticlePosition = data.getVec3FromJson(jsonEmitter["particlePosition"]);

		data.initialParticleSize = data.getVarFromJson<GLfloat>(jsonEmitter["particleSize"]);
		data.particleSizeTimeline = std::move(data.loadVarTimelineFromJson<GLfloat>(jsonEmitter["particleSize"]));

		data.initialParticleSpeed = data.getVarFromJson<GLfloat>(jsonEmitter["particleSpeed"]);
		data.initialParticleWeight = data.getVarFromJson<GLfloat>(jsonEmitter["particleWeight"]);

		data.initialParticleColor = data.getVec4FromJson(jsonEmitter["particleColor"]);
		data.particleColorTimeline = std::move(data.loadVec4TimelineFromJson(jsonEmitter["particleColor"]));

		emitter.init(data);
		emitters.push_back(emitter);
	}

	effectFile.close();
}
