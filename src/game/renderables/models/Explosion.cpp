#include "Explosion.h"

Explosion::Explosion()
: Sphere(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.0f), 10.0f, 0.0f, 0.0f, "explosion.jpg")
{
	this->inUse = false;
	this->castingShadow = false;
}

Explosion::Explosion(glm::vec4 color, glm::vec3 position, GLfloat radius, std::string textureName)
: Sphere(color, position, radius, 0.0f, 0.0f, textureName)
{
	this->inUse = false;
	this->castingShadow = false;
}


Explosion::~Explosion()
{
}

void Explosion::init()
{
	this->Sphere::init();
	this->maxRadius = 10.0f * radius;
	this->radiusGrowthSpeed = (maxRadius - radius) * 1.5f;
}

void Explosion::update(GLfloat deltaTime)
{
	radius += deltaTime * radiusGrowthSpeed;
	stableModelMatrix = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(radius));
	if (radius > maxRadius)
	{
		destroy();
	}
}

void Explosion::destroy()
{
	this->inUse = false;
	this->radius = 10.0f;
	this->translateVector = glm::vec3(0.0f, -10000.0f, 0.0f);
	this->pointLight->inUse = false;
	stableModelMatrix = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(radius));
	//pointLight->lightManager->pointLights.erase(std::remove(pointLight->lightManager->pointLights.begin(), pointLight->lightManager->pointLights.end(), pointLight), pointLight->lightManager->pointLights.end());
}