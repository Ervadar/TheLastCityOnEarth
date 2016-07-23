#include "Cannon.h"
#include <glm/gtc/constants.hpp>
#include "GameScreen.h"

Cannon::Cannon()
{
	cannonSpeed = 20.0f;

	M_PI = glm::pi<GLfloat>();
	movingAngle = (float)-sqrt(2)*glm::tan(rotateAngleX*M_PI / 180);

	initialMovingBarrelPos = 0.0f;
	movingBarrelPos = initialMovingBarrelPos;

	bulletSpeed = 1000.0f;

	bulletSpawnOffset = 175.0f;

	shootingSpeed = 125.0f;
	loadingSpeed = 40.0f;
	cannonStatus = STATUS_READY;

}


Cannon::~Cannon()
{
}

void Cannon::init(Screen * screen)
{
	this->screen = screen;
	/* KONSTRUKCJA DZIA£A */
	Renderable::init(glm::vec3(1.0f), 0.0f, 17.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(700.0f, 300.0f, 1300.0f));

	cannonInitialDirectionVector = glm::vec3(0.0f, 0.0f, -1.0f);

	// Kula podstawy
	cannonSphere = new Sphere(
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		translateVector,
		90.0f,
		rotateAngleX,
		rotateAngleY,
		"hdfield.jpg"
		);
	subObjects.push_back(cannonSphere);

	// Lufa
	barrel = new Cylinder(
		translateVector,
		glm::vec3(3.0f, 3.f, 6.0f),
		rotateAngleX,
		rotateAngleY,
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		"hdfield.jpg"
		);
	subObjects.push_back(barrel);


	// Lufa ruchoma
	movingBarrel = new Cylinder(
		translateVector,
		glm::vec3(5.0f, 5.f, 5.0f),
		rotateAngleX,
		rotateAngleY,
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		"hdfield.jpg"
		);
	subObjects.push_back(movingBarrel);

	cannonSphere->init(screen);
	// Wychylenie pocz¹tkowe cannona
	// cannonSphere->stableModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * cannonSphere->stableModelMatrix;

	barrel->init(screen);
	barrel->stableModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -120.0f)) * barrel->stableModelMatrix;

	movingBarrel->init(screen);
	movingBarrel->stableModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -113.0f)) * movingBarrel->stableModelMatrix;
}

void Cannon::render(ShaderProgram & shaderProgram)
{
}

void Cannon::update(float deltaTime)
{
	// Cannon X axis and direction update
	rotateAxisX = glm::vec3(glm::rotate(glm::mat4(1.0f), rotateAngleY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	cannonDirectionVector = glm::vec3(
		glm::rotate(glm::mat4(1.0f), rotateAngleX, rotateAxisX) *
		glm::rotate(glm::mat4(1.0f), rotateAngleY, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::vec4(cannonInitialDirectionVector, 1.0f));

	// Cannon status
	if (cannonStatus == STATUS_SHOOTING)
	{
		movingBarrelPos -= shootingSpeed * deltaTime;
		if (movingBarrelPos < initialMovingBarrelPos - 45.0f)
		{
			cannonStatus = STATUS_LOADING;
		}
	}

	if (cannonStatus == STATUS_LOADING)
	{
		movingBarrelPos += loadingSpeed*deltaTime;
		if (movingBarrelPos > initialMovingBarrelPos)
		{
			cannonStatus = STATUS_READY;
		}
	}

	// Cannon controls
	if (glfwGetKey(screen->window, GLFW_KEY_KP_4) == GLFW_PRESS)
	{
		rotateAngleY += deltaTime*cannonSpeed;
		if (rotateAngleY > 360.0f) rotateAngleY = 0.0f;
	}

	if (glfwGetKey(screen->window, GLFW_KEY_KP_6) == GLFW_PRESS)
	{
		rotateAngleY -= deltaTime*cannonSpeed;
		if (rotateAngleY < 0.0f) rotateAngleY = 360.0f;
	}

	if (glfwGetKey(screen->window, GLFW_KEY_KP_8) == GLFW_PRESS)
	{
		rotateAngleX += deltaTime*cannonSpeed;
		if (rotateAngleX > 197.0f) rotateAngleX = 197.0f;
	}

	if (glfwGetKey(screen->window, GLFW_KEY_KP_5) == GLFW_PRESS)
	{
		rotateAngleX -= deltaTime*cannonSpeed;
		if (rotateAngleX < -8.0f) rotateAngleX = -8.0f;
	}

	// Rotating cannon and its parts
	for (Renderable * cannonPart : subObjects)
	{
		cannonPart->rotateAngleX = rotateAngleX;
		cannonPart->rotateAngleY = rotateAngleY;
		cannonPart->rotateAxisX = rotateAxisX;
		cannonPart->update(deltaTime);
	}
	
	// Updating moving barrel position
	movingBarrel->translateVector = translateVector + movingBarrelPos * cannonDirectionVector;

}

void Cannon::destroy()
{
	for (Renderable * cannonPart : subObjects)
	{
		cannonPart->destroy();
		delete cannonPart;
	}
}