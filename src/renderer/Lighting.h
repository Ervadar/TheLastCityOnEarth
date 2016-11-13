#ifndef _LIGHTING_H_
#define _LIGHTING_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PointLight.h"

class PointLight;

/*
Class for handling lights
*/
class LightManager
{
public:
	static const GLuint POINT_LIGHT_POOL_SIZE = 70;
	std::vector<std::unique_ptr<PointLight>> pointLights;	// Point light pool

	LightManager() {};
	~LightManager();

	void init();

	PointLight* createPointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

};

#endif