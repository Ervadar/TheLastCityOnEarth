#include "ParticleEffect.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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

	//glGenBuffers(1, &positionBuffer);	// Positions and sizes of particles
	//glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	//glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//glGenBuffers(1, &colorBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	//glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
}

void ParticleEffect::customRender(ShaderProgram & shaderProgram)
{
	//glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	//glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLfloat), particlePositionAndSizeData);

	//glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	//glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLubyte), particleColorData);

	/*glBindVertexArray(VAO);
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);	*/
	printf("   Rendering explo\n");
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	printf("   Stopped rendering explo\n");
}

void ParticleEffect::update(float deltaTime)
{
	static float timePassed = 0.0f;

	// Setting up new particles in this frame
	GLint newParticleCount = (GLint)(deltaTime * particlesPerSecond);
	if (newParticleCount > (GLint)(0.016f * 10000))
	{
		newParticleCount = (GLint)(0.016f * 10000);
	}
	for (int i = 0; i < newParticleCount; ++i)
	{
		int particleIndex = findUnusedParticle();
		Particle& p = particles[particleIndex];
		p.life = initialParticleLife;
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

				particleColorData[particleCount * 4 + 0] = 1;
				particleColorData[particleCount * 4 + 1] = 1;
				particleColorData[particleCount * 4 + 2] = 1;
				particleColorData[particleCount * 4 + 3] = 255;

				++particleCount;
			}
			else
			{
				
			}
		}
	}

	timePassed += deltaTime;
	if (timePassed > 2.0f)
	{
		timePassed = 0.0f;
		inUse = false;
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
