#include "WorldRenderer.h"

void WorldRenderer::init(int viewportWidth, int viewportHeight)
{
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;

	// Load shaders and create shader programs
	shaderProgram.loadShaderProgram("main.vert", "main.frag");
	skyboxShaderProgram.loadShaderProgram("skybox.vert", "skybox.frag");
	terrainShaderProgram.loadShaderProgram("terrain.vert", "terrain.frag");
	screenShaderProgram.loadShaderProgram("framebuffer.vert", "framebuffer.frag");
	colorShaderProgram.loadShaderProgram("color.vert", "color.frag");

	depthCamera.setOrthographicMatrix(-1600.0f, 1600.0f, -1600.0f, 1600.0f, 0.2f, 4000.0f);
	depthCamera.init(
		nullptr,
		glm::vec3(0, 27, 50),	// Camera position
		3.14f,	// Horizontal angle
		0.0f,	// Vertical angle
		300.0f,	// Camera speed
		0.005f	// Mouse speed
		);

	shadowMapper.init(1024, 1024);
}

void WorldRenderer::render()
{
	shadowMapper.depthShaderProgram.useProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapper.shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowMapper.shadowWidth, shadowMapper.shadowHeight);
	renderObjectsCastingShadows(depthCamera, shadowMapper.depthShaderProgram, &WorldRenderer::setDepthShaderMatricesAndUniforms);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glViewport(0, 0, viewportWidth, viewportHeight);

	// Shadow mapa - nr tekstury powinien byc wiekszy niz najwiekszy uzywany w obiektach
	glActiveTexture(GL_TEXTURE0 + 31);
	glBindTexture(GL_TEXTURE_2D, shadowMapper.depthMap);

	// SKYBOX
	skyboxShaderProgram.useProgram();
	render(&(world->skybox), camera, skyboxShaderProgram, &WorldRenderer::setSkyboxShaderMatricesAndUniforms);

	// TERRAIN
	terrainShaderProgram.useProgram();
	render(&(world->terrain), camera, terrainShaderProgram, &WorldRenderer::setTerrainShaderMatricesAndUniforms);

	// OBJECTS
	shaderProgram.useProgram();
	renderObjects(camera, shaderProgram, &WorldRenderer::setMainShaderMatricesAndUniforms);

	// POINT LIGHTS
	colorShaderProgram.useProgram();
	renderPointLights(camera, colorShaderProgram, &WorldRenderer::setColorShaderMatricesAndUniforms);
}

void WorldRenderer::update(GLfloat deltaTime)
{
	updateDepthCamera(deltaTime);
}

void WorldRenderer::renderObjectsCastingShadows(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction)
{
	for (auto& object : world->staticObjects)
	{
		if (object->castingShadow) render(object.get(), camera, shaderProgram, matrixUniformFunction);
	}
	for (auto& enemyShip : world->enemyShips)
	{
		if (enemyShip->inUse && enemyShip->castingShadow) render(enemyShip.get(), camera, shaderProgram, matrixUniformFunction);
	}
	if (world->cannon->castingShadow) render(world->cannon.get(), camera, shaderProgram, matrixUniformFunction);
	if (world->forceShield->castingShadow) render(world->forceShield.get(), camera, shaderProgram, matrixUniformFunction);
}

void WorldRenderer::renderObjects(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction)
{
	for (auto& object : world->staticObjects)
	{
		render(object.get(), camera, shaderProgram, matrixUniformFunction);
	}
	for (auto& enemyShip : world->enemyShips)
	{
		if (enemyShip->inUse)
		{
			render(enemyShip.get(), camera, shaderProgram, matrixUniformFunction);
		}
	}
	for (auto& cannonMissile : world->cannonMissiles)
	{
		if (cannonMissile->inUse) render(cannonMissile.get(), camera, shaderProgram, matrixUniformFunction);
	}
	for (auto& enemyShipMissile : world->enemyShipMissiles)
	{
		if (enemyShipMissile->inUse) render(enemyShipMissile.get(), camera, shaderProgram, matrixUniformFunction);
	}
	render(world->cannon.get(), camera, shaderProgram, matrixUniformFunction);
	for (GLuint i = 0; i < world->EXPLOSION_POOL_SIZE; ++i)
	{
		if (world->explosions[i]->inUse)
		{
			render(world->explosions[i].get(), camera, shaderProgram, matrixUniformFunction);
		}
	}
	render(world->forceShield.get(), camera, shaderProgram, matrixUniformFunction);
}

void WorldRenderer::renderPointLights(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction)
{
	for (PointLight& pointLight : world->lightManager.pointLights)
	{
		if (pointLight.inUse) render(&pointLight, camera, shaderProgram, matrixUniformFunction);
	}
}

void WorldRenderer::render(Renderable * object, Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction)
{
	if (object->hasSubObjects())
	{
		for (Renderable * subObject : object->subObjects) render(subObject, camera, shaderProgram, matrixUniformFunction);
	}
	(this->*matrixUniformFunction)(object, camera, shaderProgram);
	object->render(shaderProgram);
}

void WorldRenderer::updateDepthCamera(GLfloat deltaTime)
{
	depthCamera.eye = world->sunDirection * 2.0f * (float)shadowMapper.shadowWidth;
	depthCamera.direction = world->sunDirection;
	depthCamera.setViewMatrix(depthCamera.eye, glm::vec3(0.0f), glm::vec3(1.0f));
}

void WorldRenderer::setMainShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
{
	renderable->calculateMatricesAndUniforms(camera, shaderProgram);
	shaderProgram.setUniform("shadowMap", 31);
	shaderProgram.setUniform("color", renderable->color);
	shaderProgram.setUniform("material.texture_diffuse1", 0);
	shaderProgram.setUniform("sunLight.color", world->sunLightColor);
	shaderProgram.setUniform("sunLight.directionCameraspace", glm::vec3(camera.getViewMatrix() * glm::vec4(world->sunDirection, 0.0f)));
	shaderProgram.setUniform("sunLight.ambientIntensity", world->sunLightAmbientIntensity);
	shaderProgram.setUniform("normalMatrix", glm::transpose(glm::inverse(camera.getViewMatrix() * renderable->modelMatrix)));

	// Shadows
	glm::mat4 lightSpaceMatrix = depthCamera.getProjectionMatrix() * depthCamera.getViewMatrix();
	shaderProgram.setUniform("lightSpaceMatrix", lightSpaceMatrix);

	// Point Lights
	int pointLightsNumber = 0;
	for (GLuint i = 0; i < world->lightManager.POINT_LIGHT_POOL_SIZE; ++i)
	{
		if (!world->lightManager.pointLights[i].inUse) continue;
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].position", glm::vec3(camera.getViewMatrix() * glm::vec4(world->lightManager.pointLights[i].translateVector, 1.0f)));
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].constant", world->lightManager.pointLights[i].constant);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].linear", world->lightManager.pointLights[i].linear);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].quadratic", world->lightManager.pointLights[i].quadratic);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].ambient", world->lightManager.pointLights[i].ambient);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].diffuse", world->lightManager.pointLights[i].diffuse);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].specular", world->lightManager.pointLights[i].specular);
		++pointLightsNumber;
	}
	shaderProgram.setUniform("pointLightsNumber", pointLightsNumber);
}


void WorldRenderer::setDepthShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
{
	renderable->calculateMatricesAndUniforms(camera, shaderProgram);
}

void WorldRenderer::setSkyboxShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
{
	glm::mat4 projectionMatrix = camera.getProjectionMatrix();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	viewMatrix = glm::translate(viewMatrix, camera.eye);
	glm::mat4 MVP = projectionMatrix * viewMatrix * renderable->modelMatrix;

	shaderProgram.setUniform("MVP", MVP);
	shaderProgram.setUniform("M", renderable->modelMatrix);
	shaderProgram.setUniform("V", viewMatrix);
	shaderProgram.setUniform("color", renderable->color);
	shaderProgram.setUniform("material.diffuseMap", 0);
	shaderProgram.setUniform("sunLight.ambientIntensity", 1.0f);
	shaderProgram.setUniform("normalMatrix", glm::transpose(glm::inverse(renderable->modelMatrix)));
}

void WorldRenderer::setTerrainShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
{
	Terrain* terrain = (Terrain*)renderable;

	terrain->setRenderSize(5000.0f, 1000.0f, 5000.0f);
	terrain->calculateMatricesAndUniforms(camera, shaderProgram);

	shaderProgram.setUniform("color", terrain->color);

	// Bindujemy tekstury
	for (int i = 0; i < 3; ++i)
	{
		char samplerName[256];
		sprintf_s(samplerName, "gSampler[%d]", i);
		terrain->textures[i].bindTexture(i);
		shaderProgram.setUniform(samplerName, i);
	}
	shaderProgram.setUniform("shadowMap", 31);
	shaderProgram.setUniform("sunLight.ambientIntensity", world->sunLightAmbientIntensity);
	shaderProgram.setUniform("sunLight.directionCameraspace", glm::vec3(camera.getViewMatrix() * glm::vec4(world->sunDirection, 0.0f)));
	shaderProgram.setUniform("sunLight.color", world->sunLightColor);

	shaderProgram.setUniform("renderHeight", terrain->renderScale.y);
	shaderProgram.setUniform("maxTextureU", float(terrain->cols)*0.1f);
	shaderProgram.setUniform("maxTextureV", float(terrain->rows)*0.1f);

	shaderProgram.setUniform("heightmapScaleMatrix", glm::scale(glm::mat4(1.0), glm::vec3(terrain->renderScale)));
	shaderProgram.setUniform("normalMatrix", glm::transpose(glm::inverse(camera.getViewMatrix() * terrain->modelMatrix)));

	// Shadows
	glm::mat4 lightSpaceMatrix = depthCamera.getProjectionMatrix() * depthCamera.getViewMatrix();
	shaderProgram.setUniform("lightSpaceMatrix", lightSpaceMatrix);

	// Point Lights
	int pointLightsNumber = 0;
	for (GLuint i = 0; i < world->lightManager.POINT_LIGHT_POOL_SIZE; ++i)
	{
		if (!world->lightManager.pointLights[i].inUse) continue;
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].position", glm::vec3(camera.getViewMatrix() * glm::vec4(world->lightManager.pointLights[i].translateVector, 1.0f)));
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].constant", world->lightManager.pointLights[i].constant);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].linear", world->lightManager.pointLights[i].linear);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].quadratic", world->lightManager.pointLights[i].quadratic);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].ambient", world->lightManager.pointLights[i].ambient);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].diffuse", world->lightManager.pointLights[i].diffuse);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].specular", world->lightManager.pointLights[i].specular);
		++pointLightsNumber;
	}
	shaderProgram.setUniform("pointLightsNumber", pointLightsNumber);
}

void WorldRenderer::setColorShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
{
	renderable->calculateMatricesAndUniforms(camera, shaderProgram);

	shaderProgram.setUniform("color", renderable->color);
}

void WorldRenderer::release()
{
	shaderProgram.deleteProgram();
	terrainShaderProgram.deleteProgram();
}