#include "Lighting.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PointLight.h"

LightManager::LightManager()
{
}


LightManager::~LightManager()
{
}

void LightManager::init(Screen * screen)
{
	for (GLuint i = 0; i < POINT_LIGHT_POOL_SIZE; ++i)
	{
		pointLights.push_back(PointLight(screen));
	}
}

PointLight* LightManager::createPointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	for (GLuint i = 0; i < POINT_LIGHT_POOL_SIZE; ++i)
	{
		if (!pointLights[i].inUse)
		{
			pointLights[i].init(position, constant, linear, quadratic, ambient, diffuse, specular);
			pointLights[i].inUse = true;
			return &(pointLights[i]);
		}
	}
	return nullptr;
}