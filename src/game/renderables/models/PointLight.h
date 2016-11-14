#ifndef _POINTLIGHT_H_
#define _POINTLIGHT_H_

#include "Renderable.h"
#include "Lighting.h"

class LightManager;

class PointLight : public Renderable
{
public:
	LightManager * lightManager;

	float constant;
	float linear;
	float quadratic;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	PointLight();
	PointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	~PointLight();

	void init();
	void init(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void customRender(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();
};

#endif