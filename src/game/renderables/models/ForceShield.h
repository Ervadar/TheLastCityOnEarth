#ifndef _FORCESHIELD_H_
#define _FORCESHIELD_H_

#include "Renderable.h"

class ForceShield : public Renderable
{
public:
	GLint maxHealthPoints;
	GLint healthPoints;
	GLint healthPerSecond;

	GLuint innerElementVBO;
	float rotateAngle;

	ForceShield();
	virtual ~ForceShield();

	void init();
	void customRender(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();

	void reduceHealthPoints(GLint reducedHealthPoints);
};

#endif