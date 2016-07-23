#include "GameScreen.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

GameScreen::~GameScreen()
{
	release();
}

void GameScreen::init()
{
	// Load shaders and create shader programs
	shaderProgram.loadShaderProgram("main.vert", "main.frag");
	skyboxShaderProgram.loadShaderProgram("skybox.vert", "skybox.frag");
	terrainShaderProgram.loadShaderProgram("terrain.vert", "terrain.frag");
	textShaderProgram.loadShaderProgram("text.vert", "text.frag");
	screenShaderProgram.loadShaderProgram("framebuffer.vert", "framebuffer.frag");
	colorShaderProgram.loadShaderProgram("color.vert", "color.frag");

	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	showingInstructions = false;
	showingDebugInfo = false;

	this->timePassed = 0.0f;
	this->timePassedFromLastShipSpawn = 0.0f;

	this->maxEnemyShipSpawnTime = 8.0f;
	this->minEnemyShipSpawnTime = 2.0f;

	this->enemyShipSpawnTime = maxEnemyShipSpawnTime;

	gamePaused = false;
	gameFinished = false;
	cameraCannonView = true;

	crosshairsSize = 32.0f;

	heldKeysMap[GLFW_KEY_C] = false;
	heldKeysMap[GLFW_KEY_P] = false;
	heldKeysMap[GLFW_KEY_F1] = false;
	heldKeysMap[GLFW_KEY_F3] = false;

	near = 0.2f;
	far = 4000.0f;

	camera.setPerspectiveMatrix(45.0f, (float)windowWidth / (float)windowHeight, near, far);
	camera.init(
		window,
		glm::vec3(0, 27, 50),	// Camera position
		3.14f,	// Horizontal angle
		0.0f,	// Vertical angle
		300.0f,	// Camera speed
		0.005f	// Mouse speed
		);

	depthCamera.setOrthographicMatrix(-1600.0f, 1600.0f, -1600.0f, 1600.0f, near, far);
	depthCamera.init(
		window,
		glm::vec3(0, 27, 50),	// Camera position
		3.14f,	// Horizontal angle
		0.0f,	// Vertical angle
		300.0f,	// Camera speed
		0.005f	// Mouse speed
		);

	// Sun (directional light)
	sunDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	sunLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	sunLightAmbientIntensity = 0.5f;
	sunRotateAngleX = 47.0f;
	sunRotateAngleY = 315.0f;
	sunSpeed = 50.0f;

	// Tworzymy dzia³o
	cannon = std::move(std::unique_ptr<Cannon>(new Cannon()));

	// Tworzymy miasto
	float basePosX = 50.0f;
	float basePosZ = 50.0f;
	float width = 100.0f;
	float height = 400.0f;
	float length = 100.0f;
	for (int i = 0; i < 5; ++i)
	for (int j = 0; j < 5; ++j)
	{
		staticObjects.push_back(new Cube(
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec3(basePosX - i*width * 2, 150.0f, basePosZ - j*length * 2),
			glm::vec3(width, height + (float)std::pow(-1, (i*j) % 2) * i*30.0f - (float)::pow(-1, (j*j) % 2)*j*30.0f, length),
			0.0f,
			0.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			"skyscraper" + std::to_string((j + i + i*j) % 3 + 1) + ".jpg",
			"snow.jpg",
			//"darkmetal" + std::to_string((j+i+i*j)%2+1) + ".jpg",
			5.0f,
			5.0f
			));
	}
	// Podstawa dzia³a
	int i = 0, j = 0;
	staticObjects.push_back(new Cube(
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec3(700.0f, 110.0f, 1300.0f),
		glm::vec3(200.0f, 300.0f, 200.0f),
		0.0f,
		0.0f,
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		"hdfield.jpg",
		"hdfield.jpg",
		//"darkmetal" + std::to_string((j+i+i*j)%2+1) + ".jpg",
		5.0f,
		5.0f
		));

	// Tarcza
	forceShield = std::move(std::unique_ptr<ForceShield>(new ForceShield()));
	forceShield->castingShadow = false;

	// INITIAITING
	skybox.init(this);
	terrain.init(this);
	cannon->init(this);
	for (Renderable * object : staticObjects)
	{
		object->init(this);
	}
	enemyShipSpawnPoint = glm::vec3(0.0f, 4500.0f, 0.0f);

	for (GLuint i = 0; i < ENEMY_SHIP_POOL_SIZE; ++i)
	{
		std::unique_ptr<EnemyShip> enemyShip(new EnemyShip(enemyShipSpawnPoint, &enemyShipMissiles));
		enemyShip->lightManager = &lightManager;
		enemyShip->shootingTargetPosition = forceShield->translateVector;
		// enemyShip->soundSystem = &soundSystem;
		enemyShips.push_back(std::move(enemyShip));
	}
	forceShield->init(this);
	text.init(this);
	for (int i = 0; i < 4; ++i) gameFinishedTexts[i].init(this);
	forceShieldHPtext.init(this);

	crosshairs.init(this, "crosshairs.tga");
	shadowMapper.init(1024, 1024);

	// Initiaiting enemy ship destination spheres (esds)
	glm::vec3 forceShieldPosition = forceShield->translateVector;
	glm::vec3 cannonPosition = cannon->translateVector;
	GLfloat forceShieldRadius = forceShield->radius;
	GLfloat esdsRadius = 450.0f;
	GLfloat esdsPositionZenithAngle = 40.0f;
	GLuint destinationSphereCount = 5;	// should be odd number

	GLfloat alpha = 180.0f / (destinationSphereCount + 1);
	glm::vec3 direction = cannonPosition - forceShieldPosition;
	direction.y = 0;
	direction = glm::normalize(direction);
	glm::vec3 axis = glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f));
	direction = glm::vec3(glm::rotate(glm::mat4(1.0f), esdsPositionZenithAngle, axis) * glm::vec4(direction, 0.0f));
	enemyShipDestinationSpheres.push_back(BoundingSphere(forceShieldPosition + direction*(forceShieldRadius + esdsRadius), esdsRadius));
	for (GLuint i = 0; i < (destinationSphereCount - 1) / 2; ++i)
	{
		glm::vec3 yRotatedDirection1 = glm::vec3(glm::rotate(glm::mat4(1.0f), (i + 1) * alpha, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(direction, 0.0f));
		glm::vec3 yRotatedDirection2 = glm::vec3(glm::rotate(glm::mat4(1.0f), (i + 1) * -alpha, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(direction, 0.0f));
		enemyShipDestinationSpheres.push_back(BoundingSphere(forceShieldPosition + yRotatedDirection1*(forceShieldRadius + esdsRadius), esdsRadius));
		enemyShipDestinationSpheres.push_back(BoundingSphere(forceShieldPosition + yRotatedDirection2*(forceShieldRadius + esdsRadius), esdsRadius));
	}

	// Init explosion pool
	for (GLuint i = 0; i < EXPLOSION_POOL_SIZE; ++i)
	{
		explosions.push_back(Explosion());
		explosions[i].init(this);
	}
	// Init cannon missile pool
	for (GLuint i = 0; i < CANNON_MISSILES_POOL_SIZE; ++i)
	{
		Missile* missile = new Missile();
		cannonMissiles.push_back(missile);
		missile->owner = Missile::OWNER_CANNON;
		missile->inUse = false;
	}
	// Init enemy ship missile pool
	for (GLuint i = 0; i < ENEMY_SHIP_MISSILES_POOL_SIZE; ++i)
	{
		Missile* missile = new Missile();
		enemyShipMissiles.push_back(missile);
		missile->owner = Missile::OWNER_ENEMY_SHIP;
		missile->inUse = false;
	}

	// Init light manager
	lightManager.init(this);

	// Init sound system
	// soundSystem.camera = &camera;
}

void GameScreen::release()
{
	shaderProgram.deleteProgram();
	terrainShaderProgram.deleteProgram();
	textShaderProgram.deleteProgram();
	for (auto it = staticObjects.begin(); it != staticObjects.end(); ++it)
	{
		(*it)->destroy();
		delete *it;
	}
	staticObjects.clear();
}

void GameScreen::render()
{
	shadowMapper.depthShaderProgram.useProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapper.shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowMapper.shadowWidth, shadowMapper.shadowHeight);
	renderObjectsCastingShadows(depthCamera, shadowMapper.depthShaderProgram, &GameScreen::setDepthShaderMatricesAndUniforms);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, windowWidth, windowHeight);

	// Shadow mapa - nr tekstury powinien byc wiekszy niz najwiekszy uzywany w obiektach
	glActiveTexture(GL_TEXTURE0 + 31);
	glBindTexture(GL_TEXTURE_2D, shadowMapper.depthMap);

	// SKYBOX
	skyboxShaderProgram.useProgram();
	render(&skybox, camera, skyboxShaderProgram, &GameScreen::setSkyboxShaderMatricesAndUniforms);

	// TERRAIN
	terrainShaderProgram.useProgram();
	render(&terrain, camera, terrainShaderProgram, &GameScreen::setTerrainShaderMatricesAndUniforms);

	// OBJECTS
	shaderProgram.useProgram();
	renderObjects(camera, shaderProgram, &GameScreen::setMainShaderMatricesAndUniforms);

	// POINT LIGHTS
	colorShaderProgram.useProgram();
	renderPointLights(camera, colorShaderProgram, &GameScreen::setColorShaderMatricesAndUniforms);

	// TEXT
	textShaderProgram.useProgram();
	textShaderProgram.setUniform("textureSampler", 0);
	textShaderProgram.setUniform("windowHeight", windowHeight);
	textShaderProgram.setUniform("windowWidth", windowWidth);
	text.render(0, windowHeight, 16);
	forceShieldHPtext.render((int)(windowWidth / 2 - (forceShieldHPtext.textValue.length()*0.5f) * 16), windowHeight, 16);

	// GAME FINISHED TEXT
	if (gameFinished)
	{
		for (int i = 0; i < 4; ++i)
		{
			gameFinishedTexts[i].render((int)(windowWidth / 2 - (gameFinishedTexts[i].textValue.length()*0.5f) * 32), windowHeight / 2 - i * 32, 32);
		}
	}

	// CROSSHAIRS
	if (cameraCannonView && !gameFinished)
	{
		crosshairs.render(windowWidth*0.5f - crosshairsSize*0.5f, windowHeight*0.5f - crosshairsSize*0.5f, crosshairsSize, crosshairsSize);
	}
}

void GameScreen::renderObjectsCastingShadows(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction)
{
	for (Renderable * object : staticObjects)
	{
		if (object->castingShadow) render(object, camera, shaderProgram, matrixUniformFunction);
	}
	for (auto& enemyShip : enemyShips)
	{
		if (enemyShip->inUse && enemyShip->castingShadow) render(enemyShip.get(), camera, shaderProgram, matrixUniformFunction);
	}
	if (cannon->castingShadow) render(cannon.get(), camera, shaderProgram, matrixUniformFunction);
	if (forceShield->castingShadow) render(forceShield.get(), camera, shaderProgram, matrixUniformFunction);
}

void GameScreen::renderObjects(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction)
{
	for (Renderable * object : staticObjects)
	{
		render(object, camera, shaderProgram, matrixUniformFunction);
	}
	for (auto& enemyShip : enemyShips)
	{
		if (enemyShip->inUse)
		{
			render(enemyShip.get(), camera, shaderProgram, matrixUniformFunction);
			//render(enemyShip->debugSphere, camera, shaderProgram, matrixUniformFunction);
			//for (Cube* enemyShipBoundingBox : enemyShip->debugBoundingBoxes)
			//{
			//	render(enemyShipBoundingBox, camera, shaderProgram, matrixUniformFunction);
			//}
		}
	}
	for (auto& cannonMissile : cannonMissiles)
	{
		if (cannonMissile->inUse) render(cannonMissile, camera, shaderProgram, matrixUniformFunction);
	}
	for (auto& enemyShipMissile : enemyShipMissiles)
	{
		if (enemyShipMissile->inUse) render(enemyShipMissile, camera, shaderProgram, matrixUniformFunction);
	}
	render(cannon.get(), camera, shaderProgram, matrixUniformFunction);
	for (GLuint i = 0; i < EXPLOSION_POOL_SIZE; ++i)
	{
		if (explosions[i].inUse)
		{
			render(&explosions[i], camera, shaderProgram, matrixUniformFunction);
		}
	}
	render(forceShield.get(), camera, shaderProgram, matrixUniformFunction);
}

void GameScreen::renderPointLights(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction)
{
	for (PointLight pointLight : lightManager.pointLights)
	{
		if (pointLight.inUse) render(&pointLight, camera, shaderProgram, matrixUniformFunction);
	}
}

void GameScreen::render(Renderable * object, Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction)
{
	if (object->hasSubObjects())
	{
		for (Renderable * subObject : object->subObjects) render(subObject, camera, shaderProgram, matrixUniformFunction);
	}
	(this->*matrixUniformFunction)(object, camera, shaderProgram);
	//matrixAndUniformFunction(object, camera);
	//setMainShaderMatricesAndUniforms(object, camera);
	object->render(shaderProgram);
}

void GameScreen::update(GLfloat deltaTime)
{
	checkMenuInputs();
	// HUD
	std::string debugInfo;
	if (showingDebugInfo)
	{
		debugInfo += "X: " + std::to_string(camera.eye.x) + "/";
		debugInfo += "Y: " + std::to_string(camera.eye.y) + "/";
		debugInfo += "Z: " + std::to_string(camera.eye.z) + "/";
		debugInfo += "FPS: " + std::to_string(FPSCount) + "/";
		for (GLuint i = 0; i < enemyShips.size(); ++i)
		{
			if (enemyShips[i]->inUse)
				debugInfo += "Enemy" + std::to_string(i) + " HP: " + std::to_string(enemyShips[i]->healthPoints) + "/";
		}
	}
	std::string instructions;
	if (showingInstructions)
	{
		instructions += "/Don't let the last city/on earth lose its shield!//";
		instructions += "Q - shoot cannon/";
		instructions += "C - toggle view/";
		instructions += "Arrow keys - move directional light/";
		instructions += "Numpad - move cannon/";
		instructions += "P - pause/";
		instructions += "ESC - exit/";
		instructions += "F3 - debug info/";
	}
	text.setTextValue(
		"F1 - toggle instructions/"
		+ instructions
		+ debugInfo
		);
	forceShieldHPtext.setTextValue("Shield HP: " + std::to_string(forceShield->healthPoints));

	// Checking if game is paused or finished - don't update if yes
	if (gamePaused || gameFinished) return;

	checkGameplayInputs(deltaTime);
	sunRotateAxisX = glm::vec3(glm::rotate(glm::mat4(1.0f), sunRotateAngleY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	sunDirection = glm::vec3(
		glm::rotate(glm::mat4(1.0f), sunRotateAngleX, sunRotateAxisX) *
		glm::rotate(glm::mat4(1.0f), sunRotateAngleY, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	updateDepthCamera(deltaTime);

	for (Renderable * object : staticObjects)
	{
		object->update(deltaTime);
	}
	for (auto& enemyShip : enemyShips)
	{
		if (enemyShip->inUse) enemyShip->update(deltaTime);
	}
	cannon->update(deltaTime);
	forceShield->update(deltaTime);

	// Updating missiles
	for (auto& cannonMissile : cannonMissiles)
	{
		if (cannonMissile->inUse) cannonMissile->update(deltaTime);
	}

	for (auto& enemyShipMissile : enemyShipMissiles)
	{
		if (enemyShipMissile->inUse) enemyShipMissile->update(deltaTime);
	}

	checkCollisions();

	// Updating explosions
	for (GLuint i = 0; i < EXPLOSION_POOL_SIZE; ++i)
	{
		if (explosions[i].inUse) explosions[i].update(deltaTime);
	}

	updateCamera(deltaTime);

	// Spawning enemy ships
	if (timePassedFromLastShipSpawn > enemyShipSpawnTime)
	{
		spawnEnemyShip();
		timePassedFromLastShipSpawn = 0.0f;
	}
	timePassedFromLastShipSpawn += deltaTime;

	// Increasing enemy ship spawn speed over time
	enemyShipSpawnTime = maxEnemyShipSpawnTime * ((-2.0f) *
		(1.0f - minEnemyShipSpawnTime / maxEnemyShipSpawnTime) * glm::atan(timePassed * 0.05f) / glm::pi<float>() + 1.0f);

	// Checking game end condition
	if (forceShield->healthPoints == 0)
	{
		gameFinished = true;
		gameFinishedTexts[0].setTextValue("The shield has been breached.");
		gameFinishedTexts[1].setTextValue("Humanity is hopeless now.");
		gameFinishedTexts[2].setTextValue("You lasted for " + std::to_string(timePassed) + " seconds.");
		gameFinishedTexts[3].setTextValue("Press R to restart.");
	}

	timePassed += deltaTime;
}

void GameScreen::updateCamera(GLfloat deltaTime)
{
	double xPos, yPos;
	int width, height;

	glfwGetCursorPos(window, &xPos, &yPos);
	glfwGetWindowSize(window, &width, &height);

	glfwSetCursorPos(window, width / 2, height / 2);

	// Compute new orientation
	camera.horizontalAngle += camera.mouseSpeed * float(width / 2 - xPos);
	camera.verticalAngle += camera.mouseSpeed * float(height / 2 - yPos);
	if (camera.verticalAngle > 3.14f / 2.0f) camera.verticalAngle = 3.14f / 2.0f;
	if (camera.verticalAngle < -3.14f / 2.0f) camera.verticalAngle = -3.14f / 2.0f;

	if (!cameraCannonView)
	{
		// Direction : Spherical coordinates to Cartesian coordinates conversion
		camera.direction = glm::vec3(
			cos(camera.verticalAngle) * sin(camera.horizontalAngle),
			sin(camera.verticalAngle),
			cos(camera.verticalAngle) * cos(camera.horizontalAngle)
			);

		// Right vector
		camera.right = glm::vec3(
			sin(camera.horizontalAngle - 3.14f / 2.0f),
			0,
			cos(camera.horizontalAngle - 3.14f / 2.0f)
			);

		// Up vector : perpendicular to both direction and right
		camera.up = glm::cross(camera.right, camera.direction);

		camera.updateCameraPosition(deltaTime);
	}
	else
	{
		camera.direction = cannon->cannonDirectionVector;
		camera.right = cannon->rotateAxisX;
		camera.eye = cannon->translateVector + cannon->bulletSpawnOffset * cannon->cannonDirectionVector;
		camera.up = glm::cross(camera.right, camera.direction);
	}

	camera.setViewMatrix(camera.eye, camera.eye + camera.direction, camera.up);
}

void GameScreen::updateDepthCamera(GLfloat deltaTime)
{
	depthCamera.eye = sunDirection * 2.0f * (float)shadowMapper.shadowWidth;
	depthCamera.direction = sunDirection;
	depthCamera.setViewMatrix(depthCamera.eye, glm::vec3(0.0f), glm::vec3(1.0f));
}

// INPUT

void GameScreen::checkGameplayInputs(GLfloat deltaTime)
{
	// Changing sun position
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		sunRotateAngleY += deltaTime*sunSpeed;
		if (sunRotateAngleY > 360.0f) sunRotateAngleY = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		sunRotateAngleY -= deltaTime*sunSpeed;
		if (sunRotateAngleY < 0.0f) sunRotateAngleY = 360.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		sunRotateAngleX += deltaTime*sunSpeed;
		if (sunRotateAngleX > 90.0f) sunRotateAngleX = 90.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		sunRotateAngleX -= deltaTime*sunSpeed;
		if (sunRotateAngleX < 0.0f) sunRotateAngleX = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (cannon->cannonStatus == cannon->STATUS_READY)
		{
			shootCannonMissile();
		}
	}
}

void GameScreen::checkMenuInputs()
{
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
	{
		if (!heldKeysMap[GLFW_KEY_F1])
		{
			showingInstructions = !showingInstructions;
			heldKeysMap[GLFW_KEY_F1] = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
	{
		if (!heldKeysMap[GLFW_KEY_F3])
		{
			showingDebugInfo = !showingDebugInfo;
			heldKeysMap[GLFW_KEY_F3] = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		if (!heldKeysMap[GLFW_KEY_C])
		{
			cameraCannonView = !cameraCannonView;
			heldKeysMap[GLFW_KEY_C] = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (!heldKeysMap[GLFW_KEY_P])
		{
			gamePaused = !gamePaused;
			heldKeysMap[GLFW_KEY_P] = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (gameFinished)
		{
			restartGame();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	for (auto& item : heldKeysMap)
	{
		if (glfwGetKey(window, item.first) == GLFW_RELEASE)
		{
			heldKeysMap[item.first] = false;
		}
	}
}

// COLLISIONS

void GameScreen::checkCollisions()
{
	checkMissileBoundaryCollisions(cannonMissiles);
	checkMissileEnemyShipCollisions(cannonMissiles);
	checkMissileForceShieldCollisions(cannonMissiles);
	checkMissileTerrainCollisions(cannonMissiles);
	checkMissileForceShieldCollisions(enemyShipMissiles);
}

void GameScreen::checkMissileBoundaryCollisions(std::vector<Missile*>& missiles)
{
	for (auto& missile : missiles)
	{
		if (!missile->inUse) continue;
		if (glm::abs(missile->translateVector.x) > 2000.0f || glm::abs(missile->translateVector.y) > 2000.0f || glm::abs(missile->translateVector.z) > 2000.0f)
		{
			spawnExplosion(missile->translateVector);
			missile->destroy();
		}
	}
}
void GameScreen::checkMissileEnemyShipCollisions(std::vector<Missile*>& missiles)
{
	for (auto& missile : missiles)
	{
		if (!missile->inUse) continue;
		for (auto& enemyShip : enemyShips)
		{
			if (!enemyShip->inUse) continue;
			if (missileCollidesWithObject(missile, enemyShip.get()))
			{
				GLfloat hitDistanceFromCenter = glm::distance(missile->translateVector, enemyShip->translateVector);
				GLint missileStrength = missile->strength;
				enemyShip->reduceHealthPoints(missileStrength);
				if (enemyShip->healthPoints == 0)
				{
					enemyShip->destroy();
				}
				spawnExplosion(missile->translateVector);
				// soundSystem.playSound(soundSystem.enemyShipHit,
				// { missile->translateVector.x, missile->translateVector.y, missile->translateVector.z });
				missile->destroy();
				break;
			}
		}
	}
}
void GameScreen::checkMissileForceShieldCollisions(std::vector<Missile*>& missiles)
{
	for (auto& missile : missiles)
	{
		if (!missile->inUse) continue;
		if (missileCollidesWithObject(missile, forceShield.get()))
		{
			spawnExplosion(missile->translateVector);
			forceShield->reduceHealthPoints(missile->strength);
			// soundSystem.playSound(soundSystem.shieldHit,
			// 	{ missile->translateVector.x, missile->translateVector.y, missile->translateVector.z });
			missile->destroy();
		}
	}
}
void GameScreen::checkMissileTerrainCollisions(std::vector<Missile*>& missiles)
{
	for (auto& missile : missiles)
	{
		if (!missile->inUse) continue;
		if (missile->translateVector.y < terrain.getTerrainHeight(missile->translateVector.x, missile->translateVector.z))
		{
			spawnExplosion(missile->translateVector);
			// soundSystem.playSound(soundSystem.enemyShipHit,
			// 	{ missile->translateVector.x, missile->translateVector.y, missile->translateVector.z });
			missile->destroy();
		}
	}
}

// Misisle-object collission

bool GameScreen::missileCollidesWithObject(Missile* missile, Renderable* object)
{
	if (glm::distance(missile->translateVector, object->translateVector) < missile->getBoundingSphere().radius + object->getBoundingSphere().radius)
	{
		std::vector<AABB> boundingBoxes = object->getBoundingBoxes();
		if (boundingBoxes.empty()) return true;
		else
		{
			for (AABB boundingBox : boundingBoxes)
			{
				GLfloat distanceSquared = missile->radius * missile->radius;
				glm::vec3 C1 = boundingBox.getC1(object->translateVector);
				glm::vec3 C2 = boundingBox.getC2(object->translateVector);
				glm::vec3 S = missile->translateVector;
				if (S.x < C1.x) distanceSquared -= (S.x - C1.x)*(S.x - C1.x);
				else if (S.x > C2.x) distanceSquared -= (S.x - C2.x)*(S.x - C2.x);
				if (S.y < C1.y) distanceSquared -= (S.y - C1.y)*(S.y - C1.y);
				else if (S.y > C2.y) distanceSquared -= (S.y - C2.y)*(S.y - C2.y);
				if (S.z < C1.z) distanceSquared -= (S.z - C1.z)*(S.z - C1.z);
				else if (S.z > C2.z) distanceSquared -= (S.z - C2.z)*(S.z - C2.z);
				if (distanceSquared > 0) return true;
			}
			return false;
		}
	}
	return false;
}

// GAME FUNCTIONS

void GameScreen::spawnEnemyShip()
{
	for (GLuint i = 0; i < ENEMY_SHIP_POOL_SIZE; ++i)
	{
		if (!enemyShips[i]->inUse)
		{
			enemyShips[i]->calculateCircularMovement(enemyShipDestinationSpheres);
			enemyShips[i]->movementState = EnemyShip::MOVING_TO_POSITION;
			enemyShips[i]->translateVector = enemyShipSpawnPoint;
			enemyShips[i]->inUse = true;
			return;
		}
	}
}

void GameScreen::spawnExplosion(glm::vec3 position)
{
	for (GLuint i = 0; i < EXPLOSION_POOL_SIZE; ++i)
	{
		if (!explosions[i].inUse)
		{
			explosions[i].translateVector = position;
			explosions[i].inUse = true;
			explosions[i].pointLight = lightManager.createPointLight(
				position,
				0.1f, 0.0014f, 0.000007f,
				glm::vec3(1.0f, 153.0f / 255.0f, 0.0f),
				glm::vec3(1.0f, 153.0f / 255.0f, 0.0f),
				glm::vec3(0.3f, 0.3f, 0.0f)
				);
			return;
		}
	}
}

void GameScreen::shootCannonMissile()
{
	// Calculating cannon direction (for bullets and moving barrel)
	for (int i = 0; i < CANNON_MISSILES_POOL_SIZE; ++i)
	{
		if (cannonMissiles[i]->inUse) continue;
		PointLight * pointLight = lightManager.createPointLight(
			cannon->translateVector + 175.0f * cannon->cannonDirectionVector,
			0.1f, 0.0014f, 0.000007f,
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.3f, 0.3f, 0.3f)
			);
		cannonMissiles[i]->init(cannon->translateVector + cannon->bulletSpawnOffset * cannon->cannonDirectionVector, pointLight, cannon->bulletSpeed, cannon->cannonDirectionVector);
		pointLight->lightManager = &lightManager;
		cannonMissiles[i]->inUse = true;
		cannon->cannonStatus = cannon->STATUS_SHOOTING;
		// soundSystem.playSound(soundSystem.cannonShootSound,
		// { cannon->translateVector.x, cannon->translateVector.y, cannon->translateVector.z }
		// );
		return;
	}
}

void GameScreen::restartGame()
{
	gameFinished = false;
	forceShield->healthPoints = forceShield->maxHealthPoints;
	for (auto& enemyShip : enemyShips) enemyShip->destroy();
	for (auto& cannonMissile : cannonMissiles) cannonMissile->inUse = false;
	for (auto& enemyShipMissile : enemyShipMissiles) enemyShipMissile->inUse = false;
	for (GLuint i = 0; i < explosions.size(); ++i) explosions[i].inUse = false;
	for (GLuint i = 0; i < lightManager.pointLights.size(); ++i) lightManager.pointLights[i].inUse = false;
	timePassed = 0.0f;
	enemyShipSpawnTime = maxEnemyShipSpawnTime;
}

// MATRIX AND UNIFORM FUNCTIONS

void GameScreen::setMainShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
{
	renderable->calculateMatricesAndUniforms(camera, shaderProgram);
	shaderProgram.setUniform("shadowMap", 31);
	shaderProgram.setUniform("color", renderable->color);
	shaderProgram.setUniform("material.texture_diffuse1", 0);
	shaderProgram.setUniform("sunLight.color", sunLightColor);
	shaderProgram.setUniform("sunLight.directionCameraspace", glm::vec3(camera.getViewMatrix() * glm::vec4(sunDirection, 0.0f)));
	shaderProgram.setUniform("sunLight.ambientIntensity", sunLightAmbientIntensity);
	shaderProgram.setUniform("normalMatrix", glm::transpose(glm::inverse(camera.getViewMatrix() * renderable->modelMatrix)));

	// Shadows
	glm::mat4 lightSpaceMatrix = depthCamera.getProjectionMatrix() * depthCamera.getViewMatrix();
	shaderProgram.setUniform("lightSpaceMatrix", lightSpaceMatrix);

	// Point Lights
	int pointLightsNumber = 0;
	for (GLuint i = 0; i < lightManager.POINT_LIGHT_POOL_SIZE; ++i)
	{
		if (!lightManager.pointLights[i].inUse) continue;
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].position", glm::vec3(camera.getViewMatrix() * glm::vec4(lightManager.pointLights[i].translateVector, 1.0f)));
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].constant", lightManager.pointLights[i].constant);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].linear", lightManager.pointLights[i].linear);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].quadratic", lightManager.pointLights[i].quadratic);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].ambient", lightManager.pointLights[i].ambient);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].diffuse", lightManager.pointLights[i].diffuse);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].specular", lightManager.pointLights[i].specular);
		++pointLightsNumber;
	}
	shaderProgram.setUniform("pointLightsNumber", pointLightsNumber);
}


void GameScreen::setDepthShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
{
	renderable->calculateMatricesAndUniforms(camera, shaderProgram);
}

void GameScreen::setSkyboxShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
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

void GameScreen::setTerrainShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
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
	shaderProgram.setUniform("sunLight.ambientIntensity", sunLightAmbientIntensity);
	shaderProgram.setUniform("sunLight.directionCameraspace", glm::vec3(camera.getViewMatrix() * glm::vec4(sunDirection, 0.0f)));
	shaderProgram.setUniform("sunLight.color", sunLightColor);

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
	for (GLuint i = 0; i < lightManager.POINT_LIGHT_POOL_SIZE; ++i)
	{
		if (!lightManager.pointLights[i].inUse) continue;
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].position", glm::vec3(camera.getViewMatrix() * glm::vec4(lightManager.pointLights[i].translateVector, 1.0f)));
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].constant", lightManager.pointLights[i].constant);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].linear", lightManager.pointLights[i].linear);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].quadratic", lightManager.pointLights[i].quadratic);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].ambient", lightManager.pointLights[i].ambient);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].diffuse", lightManager.pointLights[i].diffuse);
		shaderProgram.setUniform("pointLights[" + std::to_string(pointLightsNumber) + "].specular", lightManager.pointLights[i].specular);
		++pointLightsNumber;
	}
	shaderProgram.setUniform("pointLightsNumber", pointLightsNumber);
}

void GameScreen::setColorShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram)
{
	renderable->calculateMatricesAndUniforms(camera, shaderProgram);

	shaderProgram.setUniform("color", renderable->color);
}