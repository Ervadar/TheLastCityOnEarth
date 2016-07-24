#ifndef _SHADOW_MAPPER_H_
#define _SHADOW_MAPPER_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

#include "Shaders.h"
#include "Renderable.h"

/*
Class wrapping shadow map
*/
class ShadowMapper
{
public:
	GLsizei shadowWidth, shadowHeight;

	GLuint shadowMapFBO;
	GLuint depthMap;

public:
	ShaderProgram depthShaderProgram;
	Shader depthVertexShader;
	Shader depthFragmentShader;

	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

public:
	ShadowMapper() {};
	~ShadowMapper() {};

	void init(GLsizei shadowWidth, GLsizei shadowHeight);
};

#endif