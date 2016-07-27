#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

#include "Sphere.h"
#include "PointLight.h"

class Explosion :
	public Sphere
{
public:
	GLuint healthPoints;

	GLfloat maxRadius;
	GLfloat radiusGrowthSpeed;
	PointLight * pointLight;
	bool inUse;

	Explosion();
	Explosion(glm::vec4 color, glm::vec3 position, GLfloat radius, std::string textureName);
	~Explosion();

	void init();
	void update(GLfloat deltaTime);
	void destroy();
};

#endif