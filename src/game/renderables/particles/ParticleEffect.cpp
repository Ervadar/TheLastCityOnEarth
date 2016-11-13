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
		data.maxParticles = jsonEmitter["maxParticles"];
		data.particleTextureName = jsonEmitter["particleTextureName"].get<std::string>();
		data.lifeTimeInSeconds = jsonEmitter["lifeTimeInSeconds"];
		data.particlesPerSecond = jsonEmitter["particlesPerSecond"];
		data.initialParticleLife = jsonEmitter["particleLife"];
		data.initialParticlePosition = glm::vec3(jsonEmitter["particlePosition"][0], jsonEmitter["particlePosition"][1], jsonEmitter["particlePosition"][2]);
		if (jsonEmitter["rotationType"] == "random_facing_camera")
		{
			data.rotationType = ParticleEmitterData::ROTATION_RANDOM_FACING_CAMERA;
		}
		else if (jsonEmitter["rotationType"] == "into_direction")
		{
			data.rotationType = ParticleEmitterData::ROTATION_INTO_DIRECTION;
		}
		else if (jsonEmitter["rotationType"] == "facing_up")
		{
			data.rotationType = ParticleEmitterData::ROTATION_FACING_UP;
		}
		else
		{
			data.rotationType = ParticleEmitterData::ROTATION_DEFAULT;
		}

		data.size.initialValue = data.getVarFromJson<GLfloat>(jsonEmitter["particleSize"]);
		data.size.timeline = std::move(data.loadVarTimelineFromJson<GLfloat>(jsonEmitter["particleSize"]));

		data.speed.initialValue = data.getVarFromJson<GLfloat>(jsonEmitter["particleSpeed"]);
		data.weight.initialValue = data.getVarFromJson<GLfloat>(jsonEmitter["particleWeight"]);

		data.color.initialValue = data.getVec4FromJson(jsonEmitter["particleColor"]);
		data.color.timeline = std::move(data.loadVec4TimelineFromJson(jsonEmitter["particleColor"]));

		emitter.init(data);
		emitters.push_back(emitter);
	}

	effectFile.close();
}
