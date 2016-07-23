#ifndef _SHADER_UNIFORMS_H_
#define _SHADER_UNIFORMS_H_

#include "Renderable.h"
#include "Camera.h"
#include "Shaders.h"

void setupMainShaderUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
{
	glm::mat4 projectionMatrix = camera.getProjectionMatrix();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	glm::mat4 MVP = projectionMatrix * viewMatrix * renderable->modelMatrix;

	shaderProgram.setUniform("MVP", MVP);
	shaderProgram.setUniform("M", renderable->modelMatrix);
	shaderProgram.setUniform("V", viewMatrix);
	shaderProgram.setUniform("color", renderable->color);
	shaderProgram.setUniform("sampler", 0);
	shaderProgram.setUniform("sunLight.vColor", screen->sunLightColor);
	shaderProgram.setUniform("sunLight.vDirection", screen->sunDirection);
	shaderProgram.setUniform("sunLight.fAmbientIntensity", screen->sunLightAmbientIntensity);
	shaderProgram.setUniform("normalMatrix", glm::transpose(glm::inverse(modelMatrix)));
}

#endif