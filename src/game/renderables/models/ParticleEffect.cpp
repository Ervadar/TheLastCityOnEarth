#include "ParticleEffect.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

extern std::mt19937 rng;

ParticleEffect::ParticleEffect()
{
}

ParticleEffect::~ParticleEffect()
{
}

void ParticleEffect::init()
{
	printf("Initializing particle effect\n");
	color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

	inUse = false;
	this->Renderable::init(glm::vec3(0.0f), glm::vec3(10.0f));

	glm::vec3 vertexData[] = {
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f, 0.5f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.0f)
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexVBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &positionBuffer);	// Positions and sizes of particles
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
}

void ParticleEffect::customRender(ShaderProgram & shaderProgram)
{
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLfloat), particlePositionAndSizeData);

	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLubyte), particleColorData);

	glBindVertexArray(VAO);
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	glEnable(GL_BLEND);
	glDepthMask(0);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);
	
	glDisable(GL_BLEND);
	glDepthMask(1);
}

void ParticleEffect::update(float deltaTime)
{
	// Setting up new particles in this frame (only if effect still active)
	if (active)
	{
		GLint newParticleCount = (GLint)(deltaTime * particlesPerSecond);
		if (newParticleCount > (GLint)(0.016f * particlesPerSecond))
		{
			newParticleCount = (GLint)(0.016f * particlesPerSecond);
		}
		for (int i = 0; i < newParticleCount; ++i)
		{
			int particleIndex = findUnusedParticle();
			Particle& p = particles[particleIndex];
			p.life = initialParticleLife;
			p.position = glm::vec3(0, 0, 0);
			std::uniform_int_distribution<GLint> randomColor(0, 255);
			p.r = randomColor(rng);
			p.g = randomColor(rng);
			p.b = randomColor(rng);
			p.a = 127;
		}
	}

	// Updating particles
	particleCount = 0;
	for (int i = 0; i < maxParticles; ++i)
	{
		Particle& p = particles[i];

		if (p.isAlive())
		{
			p.life -= deltaTime * lifeDecreasingPerSecond;

			if (p.isAlive())
			{
				p.position += p.speed * deltaTime;

				// Updating GPU buffers
				particlePositionAndSizeData[particleCount * 4 + 0] = p.position.x;
				particlePositionAndSizeData[particleCount * 4 + 1] = p.position.y;
				particlePositionAndSizeData[particleCount * 4 + 2] = p.position.z;
				particlePositionAndSizeData[particleCount * 4 + 3] = p.size;

				particleColorData[particleCount * 4 + 0] = p.r;
				particleColorData[particleCount * 4 + 1] = p.g;
				particleColorData[particleCount * 4 + 2] = p.b;
				particleColorData[particleCount * 4 + 3] = p.a;

				++particleCount;
			}
		}
	}

	timePassed += deltaTime;
	if (timePassed > 2.0f && active)
	{
		active = false;
	}
	if (particleCount == 0)
	{
		inUse = false;
		timePassed = 0.0f;
	}
}

void ParticleEffect::destroy()
{
}

int ParticleEffect::findUnusedParticle()
{
	for (int i = lastUsedParticle; i < maxParticles; ++i)
	{
		if (particles[i].life < 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	for (int i = 0; i < lastUsedParticle; ++i)
	{
		if (particles[i].life < 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	return 0;
}

void ParticleEffect::updateAllParticleCameraDistances(Camera& camera)
{
	for (int i = 0; i < maxParticles; ++i)
	{
		Particle& p = particles[i];

		if (p.isAlive())
		{
			p.distanceFromCamera = glm::length(p.position - camera.eye);
		}
		else
		{
			p.distanceFromCamera = -1.0f;
		}
	}
}

void ParticleEffect::activate()
{
	active = true;
}
