#include "GameScreen.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void GameScreen::init()
{
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	showingInstructions = false;
	showingDebugInfo = false;

	cameraCannonView = true;

	crosshairsSize = 32.0f;

	heldKeysMap[GLFW_KEY_C] = false;
	heldKeysMap[GLFW_KEY_P] = false;
	heldKeysMap[GLFW_KEY_F1] = false;
	heldKeysMap[GLFW_KEY_F3] = false;

	// Creating camera
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	camera.setPerspectiveMatrix(45.0f, (float)windowWidth / (float)windowHeight, 0.2f, 4000.0f);
	camera.init(
		window,
		glm::vec3(0, 27, 50),	// Camera position
		3.14f,	// Horizontal angle
		0.0f,	// Vertical angle
		300.0f,	// Camera speed
		0.01f	// Mouse speed
		);
	
	// Creating world
	world = std::move(std::unique_ptr<World>(new World()));
	world->init();
	renderer = std::move(std::unique_ptr<WorldRenderer>(new WorldRenderer(world.get(), camera)));
	renderer->init(windowWidth, windowHeight);

	text.init();
	gameFinishedText.init();
	forceShieldHPtext.init();

	crosshairs.init("crosshairs.tga");

	textShaderProgram.loadShaderProgram("text.vert", "text.frag");

	// Init sound system
	SoundSystem::getInstance().setPlayerCamera(&camera);
}

void GameScreen::release()
{
	world->release();
	renderer->release();
}

void GameScreen::render()
{
	renderer->render();

	// TEXT
	textShaderProgram.useProgram();
	textShaderProgram.setUniform("textureSampler", 0);
	textShaderProgram.setUniform("windowHeight", windowHeight);
	textShaderProgram.setUniform("windowWidth", windowWidth);
	text.render(0, windowHeight, 16);
	forceShieldHPtext.render((int)(windowWidth / 2 - (forceShieldHPtext.textValue.length()*0.5f) * 16), windowHeight, 16);

	// GAME FINISHED TEXT
	if (world->gameFinished)
	{
		std::string text = "The shield has been breached./";
		text += "Humanity is hopeless now.//";
		text += "You lasted for " + std::to_string(world->timePassed) + " seconds./";
		text += "Press R to restart.";
		gameFinishedText.setTextValue(text);
		gameFinishedText.render((int)(windowWidth / 2 - (gameFinishedText.textWidthInChars*0.5f) * 32),
			((int)(windowHeight / 2 - (gameFinishedText.numberOfLines*0.5f))), 32);
	}

	// CROSSHAIRS
	if (cameraCannonView && !world->gameFinished)
	{
		crosshairs.render(windowWidth*0.5f - crosshairsSize*0.5f, windowHeight*0.5f - crosshairsSize*0.5f, crosshairsSize, crosshairsSize);
	}
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
		for (GLuint i = 0; i < world->enemyShips.size(); ++i)
		{
			if (world->enemyShips[i]->inUse)
				debugInfo += "Enemy" + std::to_string(i) + " HP: " + std::to_string(world->enemyShips[i]->healthPoints) + "/";
		}
	}
	std::string instructions;
	if (showingInstructions)
	{
		instructions += "/Don't let the last city/on earth lose its shield!//";
		instructions += "Mouse - move cannon/";
		instructions += "LMB - shoot cannon/";
		instructions += "C - toggle view/";
		instructions += "Arrow keys - move directional light/";
		instructions += "P - pause/";
		instructions += "ESC - exit/";
		instructions += "F3 - debug info/";
	}
	text.setTextValue(
		"F1 - toggle instructions/"
		+ instructions
		+ debugInfo
		);
	forceShieldHPtext.setTextValue("Shield HP: " + std::to_string(world->forceShield->healthPoints));

	// Checking if game is paused or finished - don't update if yes
	if (world->gameFinished) return;
	if (world->gamePaused) return;

	checkGameplayInputs(deltaTime);

	world->update(deltaTime);
	renderer->update(deltaTime);

	updateCamera(deltaTime);
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

	if (cameraCannonView)
	{
		if (camera.verticalAngle > world->cannon->maxVerticalAngle) camera.verticalAngle = world->cannon->maxVerticalAngle;
		if (camera.verticalAngle < world->cannon->minVerticalAngle) camera.verticalAngle = world->cannon->minVerticalAngle;
		if (camera.horizontalAngle > world->cannon->maxHorizontalAngle) camera.horizontalAngle = world->cannon->minHorizontalAngle;
		if (camera.horizontalAngle < world->cannon->minHorizontalAngle) camera.horizontalAngle = world->cannon->maxHorizontalAngle;
		// Update cannon orientation
		world->cannon->updateRotation(camera.verticalAngle, camera.horizontalAngle);
	}

	if (!cameraCannonView)
	{
		if (camera.verticalAngle > 3.14f / 2.0f) camera.verticalAngle = 3.14f / 2.0f;
		if (camera.verticalAngle < -3.14f / 2.0f) camera.verticalAngle = -3.14f / 2.0f;

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
		camera.direction = world->cannon->cannonDirectionVector;
		camera.right = world->cannon->rotateAxisX;
		camera.eye = world->cannon->translateVector + world->cannon->bulletSpawnOffset * world->cannon->cannonDirectionVector;
		camera.up = glm::cross(camera.right, camera.direction);
	}

	camera.setViewMatrix(camera.eye, camera.eye + camera.direction, camera.up);
}

// INPUT

void GameScreen::checkGameplayInputs(GLfloat deltaTime)
{
	// Changing sun position
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		world->sunRotateAngleY += deltaTime*world->sunSpeed;
		if (world->sunRotateAngleY > 360.0f) world->sunRotateAngleY = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		world->sunRotateAngleY -= deltaTime*world->sunSpeed;
		if (world->sunRotateAngleY < 0.0f) world->sunRotateAngleY = 360.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		world->sunRotateAngleX += deltaTime*world->sunSpeed;
		if (world->sunRotateAngleX > 90.0f) world->sunRotateAngleX = 90.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		world->sunRotateAngleX -= deltaTime*world->sunSpeed;
		if (world->sunRotateAngleX < 0.0f) world->sunRotateAngleX = 0.0f;
	}

	// Cannon shooting
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (world->cannon->cannonStatus == world->cannon->STATUS_READY)
		{
			world->shootCannonMissile();
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
			world->gamePaused = !world->gamePaused;
			heldKeysMap[GLFW_KEY_P] = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (world->gameFinished)
		{
			world->reset();
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
