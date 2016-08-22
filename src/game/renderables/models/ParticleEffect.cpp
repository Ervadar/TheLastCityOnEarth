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
	glGenVertexArrays(1, &VAO);
}

void ParticleEffect::customRender(ShaderProgram & shaderProgram)
{
}

void ParticleEffect::update(float deltaTime)
{
}

void ParticleEffect::destroy()
{
}
