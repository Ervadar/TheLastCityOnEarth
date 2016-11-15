#ifndef _PARTICLEEMITTER_H_
#define _PARTICLEEMITTER_H_

#include "Renderable.h"
#include "Particle.h"
#include <random>
#include "json.hpp"

template<typename Typename>
class ParticleVariable
{
public:
	Typename initialValue;
	std::vector<std::pair<GLfloat, Typename>> timeline;

	void update(Typename& particleVariable, GLfloat particleLife)
	{
		if (timeline.empty()) return;
		GLfloat startTime = 0.0f;
		Typename startValue = initialValue;
		GLfloat endTime = 0.0f;
		Typename endValue = initialValue;
		for (unsigned int timePointIdx = 0; timePointIdx < timeline.size(); ++timePointIdx)
		{
			if (particleLife > timeline[timePointIdx].first)
			{
				startTime = timeline[timePointIdx].first;
				startValue = timeline[timePointIdx].second;
				endTime = timeline[timePointIdx].first;
				endValue = timeline[timePointIdx].second;
			}
			else
			{
				endTime = timeline[timePointIdx].first;
				endValue = timeline[timePointIdx].second;
				break;
			}
		}
		GLfloat timeDelta = endTime - startTime;
		if (timeDelta > 0.0f)
		{
			Typename valueDelta = endValue - startValue;
			GLfloat timePassedFromLastPoint = particleLife - startTime;
			particleVariable = startValue + (timePassedFromLastPoint / timeDelta) * valueDelta;
		}
	}
};

class ParticleEmitterData
{
public:
	static const GLint ROTATION_DEFAULT = 1;
	static const GLint ROTATION_RANDOM_FACING_CAMERA = 2;
	static const GLint ROTATION_INTO_DIRECTION = 3;
	static const GLint ROTATION_FACING_UP = 4;

	GLuint maxParticles;
	std::string particleTextureName;
	GLfloat lifeTimeInSeconds;
	GLuint particlesPerSecond;
	GLfloat initialParticleLife;
	glm::vec3 initialParticlePosition;
	GLint rotationType;

	ParticleVariable<GLfloat> size;
	ParticleVariable<GLfloat> speed;
	ParticleVariable<GLfloat> weight;
	ParticleVariable<glm::vec4> color;

public:
	ParticleEmitterData() {};

	template<typename ReturnType>
	ReturnType getVarFromJson(nlohmann::basic_json<>& json)
	{
		ReturnType loadedVariable(json["initial"]);
		return loadedVariable;
	}

	template<typename ReturnType>
	std::vector<std::pair<GLfloat, ReturnType>> loadVarTimelineFromJson(nlohmann::basic_json<>& json)
	{
		std::vector<std::pair<GLfloat, ReturnType>> varTimeline;
		for (unsigned int i = 0; i < json["timeline"].size(); ++i)
		{
			varTimeline.push_back(std::make_pair(json["timeline"][i]["t"], json["timeline"][i]["v"]));
		}
		return varTimeline;
	}

	glm::vec3 getVec3FromJson(nlohmann::basic_json<>& json)
	{
		return glm::vec3(json["initial"][0], json["initial"][1], json["initial"][2]);
	}

	glm::vec4 getVec4FromJson(nlohmann::basic_json<>& json)
	{
		return glm::vec4(json["initial"][0], json["initial"][1], json["initial"][2], json["initial"][3]);
	}

	std::vector<std::pair<GLfloat, glm::vec4>> loadVec4TimelineFromJson(nlohmann::basic_json<>& json)
	{
		std::vector<std::pair<GLfloat, glm::vec4>> varTimeline;
		for (unsigned int i = 0; i < json["timeline"].size(); ++i)
		{
			glm::vec4 variableVec4(json["timeline"][i]["v"][0], json["timeline"][i]["v"][1], json["timeline"][i]["v"][2], json["timeline"][i]["v"][3]);
			varTimeline.push_back(std::make_pair(json["timeline"][i]["t"], variableVec4));
		}
		return varTimeline;
	}

};

class ParticleEmitter
{
private:
	//static const int MAX_PARTICLES = 1000;
	static const glm::vec3 GRAVITY;

	GLuint particleCount = 0;
	GLboolean generateNewParticles = true;
	int lastUsedParticle = 0;

	std::vector<Particle> particles;
	std::vector<glm::vec4> particleRotationData;
	std::vector<GLfloat> particlePositionAndSizeData;
	std::vector<GLfloat> particleColorData;

	ParticleEmitterData emitterData;
	Texture particleTexture;
	std::vector<glm::vec3> vertexData;

	GLuint uvBuffer;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint rotationBuffer;

	GLuint VAO;
	GLuint vertexVBO;

	GLdouble generationTimeAccumulator = 0.0f;

public:
	ParticleEmitter(ParticleEmitterData data);
	~ParticleEmitter();

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