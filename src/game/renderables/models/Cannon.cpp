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

	bulletSpeed = 2000.0f;

	bulletSpawnOffset = 175.0f;

	shootingSpeed = 200.0f;
	loadingSpeed = 80.0f;
	status = Status::READY;
}

void Cannon::init()
{
	/* KONSTRUKCJA DZIA£A */
	Renderable::init(glm::vec3(1.0f), 0.0f, 17.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(700.0f, 300.0f, 1300.0f));

	cannonInitialDirectionVector = glm::vec3(0.0f, 0.0f, -1.0f);

	// Kula podstawy
	cannonSphere = std::move(std::unique_ptr<Sphere>(new Sphere(
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		translateVector,
		90.0f,
		rotateAngleX,
		rotateAngleY,
		"hdfield.jpg"
		)));
	subObjects.push_back(cannonSphere.get());

	// Lufa
	barrel = std::move(std::unique_ptr<Cylinder>(new Cylinder(
		translateVector,
		glm::vec3(3.0f, 3.f, 6.0f),
		rotateAngleX,
		rotateAngleY,
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		"hdfield.jpg"
		)));
	subObjects.push_back(barrel.get());


	// Lufa ruchoma
	movingBarrel = std::move(std::unique_ptr<Cylinder>(new Cylinder(
		translateVector,
		glm::vec3(5.0f, 5.f, 5.0f),
		rotateAngleX,
		rotateAngleY,
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		"hdfield.jpg"
		)));
	subObjects.push_back(movingBarrel.get());

	cannonSphere->init();
	// Wychylenie pocz¹tkowe dzia³a
	// cannonSphere->stableModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * cannonSphere->stableModelMatrix;

	barrel->init();
	barrel->stableModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -120.0f)) * barrel->stableModelMatrix;

	movingBarrel->init();
	movingBarrel->stableModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -113.0f)) * movingBarrel->stableModelMatrix;
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
	if (status == Status::SHOOTING)
	{
		movingBarrelPos -= shootingSpeed * deltaTime;
		if (movingBarrelPos < initialMovingBarrelPos - 45.0f)
		{
			status = Status::LOADING;
		}
	}

	if (status == Status::LOADING)
	{
		movingBarrelPos += loadingSpeed*deltaTime;
		if (movingBarrelPos > initialMovingBarrelPos)
		{
			status = Status::READY;
		}
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

void Cannon::updateRotation(GLfloat rotateAngleX, GLfloat rotateAngleY)
{
	this->rotateAngleX = rotateAngleX;
	if (this->rotateAngleX > 90.0f) this->rotateAngleX = 90.0f;
	if (this->rotateAngleX < -8.0f) this->rotateAngleX = -8.0f;

	this->rotateAngleY = rotateAngleY;
	if (this->rotateAngleY > 360.0f) this->rotateAngleY = 0.0f;
	if (this->rotateAngleY < 0.0f) this->rotateAngleY = 360.0f;
}
