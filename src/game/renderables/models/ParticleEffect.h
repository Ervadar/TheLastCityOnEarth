#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Renderable.h"

class ParticleEffect : Renderable
{
public:
	ParticleEffect();
	virtual ~ParticleEffect();

	void init();
	virtual void customRender(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();
};

#endif