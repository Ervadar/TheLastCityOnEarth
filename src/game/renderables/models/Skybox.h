#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "Renderable.h"
#include <GLFW/glfw3.h>

class Skybox : public Renderable
{
private:
	Texture textures[6];

public:
	Skybox();
	virtual ~Skybox();

	void init();
	void customRender(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();
};

#endif
