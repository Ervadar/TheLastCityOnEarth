#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "World.h"
#include "Camera.h"
#include "ShadowMapper.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


/*
The renderer of the game world
*/
class WorldRenderer
{
private:
	World& world;
	
	ShadowMapper shadowMapper;

	// Camera reference
	Camera & camera;

	int viewportWidth;
	int viewportHeight;

	Camera depthCamera;

	// Shader programs
	ShaderProgram shaderProgram, skyboxShaderProgram, terrainShaderProgram,
		screenShaderProgram, colorShaderProgram, particleShaderProgram;

public:
	WorldRenderer(World& world, Camera& camera) : world(world), camera(camera) {}
	void init(int viewportWidth, int viewportHeight);
	void render();
	void update(GLfloat deltaTime);
	void release();

	void updateDepthCamera(GLfloat deltaTime);

	// Function ptr type for setting matrices and uniforms
	typedef void (WorldRenderer::*matrixUniformFunction)(Renderable*, Camera&, ShaderProgram&);

	// Matrix and Uniform functions
	void setMainShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
	void setTerrainShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
	void setSkyboxShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
	void setDepthShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
	void setColorShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
	void setParticleShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);

	void render(Renderable * object, Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction);
	void renderObjects(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction);
	void renderPointLights(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction);
	void renderObjectsCastingShadows(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction);
};

#endif