#include "EnemyShip.h"
#include <iostream>

extern std::mt19937 rng;

EnemyShip::EnemyShip(glm::vec3 position, std::vector<Missile*>* missilePool)
{
	init("data/models/enemyship.obj", position, glm::vec3(50.0f));
	this->spawnPoint = position;
	this->speed = 1.0f;
	this->rotationSpeed = 60.0f;
	this->movementState = MOVING_TO_POSITION;
	this->timeBetweenShoots = 2.0f;
	this->timePassed = 0.0f;
	this->shootingTimer = 0.0f;
	this->radius = 144.0f;
	this->maxHealthPoints = 20;
	this->healthPoints = maxHealthPoints;
	this->inUse = false;
	this->loadingMissileOffset = glm::vec3(0.0f, -50.0f, 0.0f);
	this->shootingSpeed = 1000.0f;
	this->missilePool = missilePool;
	// Center bounding box
	boundingBoxes.push_back(AABB(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(80.0f, 50.0f, 80.0f)));
	// Flat bounding box
	boundingBoxes.push_back(AABB(glm::vec3(0.0f), glm::vec3(2 * radius, 18.0f, 2 * radius)));

	// DEBUG BOUNDING SPHERE
	//debugSphere = new Sphere(
	//	glm::vec4(1.0f, 1.0f, 1.0f, 0.5f),
	//	getBoundingSphere().position,
	//	getBoundingSphere().radius,
	//	0.0f,
	//	0.0f,
	//	"hdfield.jpg"
	//	);
	//debugSphere->castingShadow = false;
	//debugSphere->init(NULL);

	// DEBUG BOUNDING CUBES
	//for (AABB boundingBox : boundingBoxes)
	//{
	//	Cube* cube = new Cube(
	//		glm::vec4(1.0f, 1.0f, 1.0f, 0.5f),
	//		translateVector + boundingBox.positionOffset,
	//		boundingBox.scale,
	//		0.0f,
	//		0.0f,
	//		glm::vec3(1.0f, 0.0f, 0.0f),
	//		glm::vec3(0.0f, 1.0f, 0.0f),
	//		"hdfield.jpg",
	//		"hdfield.jpg",
	//		5.0f,
	//		5.0f
	//		);
	//	cube->positionOffset = boundingBox.positionOffset;
	//	cube->castingShadow = false;
	//	cube->init(NULL);
	//	debugBoundingBoxes.push_back(cube);
	//}

}


EnemyShip::~EnemyShip()
{
}


void EnemyShip::update(float deltaTime)
{
	if (!inUse) return;
	if (this->movementState == MOVING_AROUND_POINT)
	{
		translateVector = getMovementCirclePosition(timePassed);
		timePassed += deltaTime;
		shootingTimer += deltaTime;
		if (shootingTimer > timeBetweenShoots)
		{
			shootingTimer = 0.0f;
			shootMissile();
		}
	}	
	if (this->movementState == MOVING_TO_POSITION)
	{
		glm::vec3 step = movementToPositionDirection * 400.0f * speed * deltaTime;
		translateVector += step;
		if (glm::distance(movementStartingPosition, translateVector) <= glm::length(step))
		{
			translateVector = movementStartingPosition;
			movementState = MOVING_AROUND_POINT;
		}
	}
	rotateAngleY -= deltaTime * rotationSpeed;

	// UPDATING DEBUG SPHERE AND BOXES
	//debugSphere->translateVector = translateVector;
	//
	//for (Cube * debugBoundingBox : debugBoundingBoxes)
	//{
	//	debugBoundingBox->translateVector = translateVector + debugBoundingBox->positionOffset;
	//}
}


void EnemyShip::calculateCircularMovement(std::vector<BoundingSphere>& enemyShipDestinationSpheres)
{
	std::uniform_int_distribution<GLuint> randomShipDestinationSphere(0, enemyShipDestinationSpheres.size()-1);
	BoundingSphere targetSphere = enemyShipDestinationSpheres[randomShipDestinationSphere(rng)];
	std::uniform_real_distribution<GLfloat> randomCircularMovementRadius(0.0f, targetSphere.radius);
	movementCircleRadius = randomCircularMovementRadius(rng);
	std::uniform_real_distribution<GLfloat> randomTargetCircleCenterOffset(movementCircleRadius + this->radius - targetSphere.radius, targetSphere.radius - movementCircleRadius - this->radius);
	movementCircleCenter = targetSphere.position + glm::vec3(
		randomTargetCircleCenterOffset(rng), randomTargetCircleCenterOffset(rng), randomTargetCircleCenterOffset(rng)
		);
	std::uniform_real_distribution<GLfloat> randomAngle(0.0f, 360.0f);
	movementPolarAngle = randomAngle(rng);
	movementAzimuthalAngle = randomAngle(rng);
	movementStartingPosition = getMovementCirclePosition(0.0f);
	movementToPositionDirection = glm::normalize(movementStartingPosition - translateVector);
}

glm::vec3 EnemyShip::getMovementCirclePosition(GLfloat timePassed)
{
	glm::vec3 position;
	position.x = movementCircleRadius * glm::cos(timePassed*speed) * -glm::sin(movementPolarAngle) +
		movementCircleRadius * glm::sin(timePassed*speed) * glm::cos(movementPolarAngle) * glm::cos(movementAzimuthalAngle) +
		movementCircleCenter.x;
	position.y = movementCircleRadius * glm::cos(timePassed*speed) * glm::cos(movementPolarAngle) + +
		movementCircleRadius * glm::sin(timePassed*speed) * glm::cos(movementAzimuthalAngle) * glm::sin(movementPolarAngle) +
		movementCircleCenter.y;
	position.z = 0 +
		movementCircleRadius * glm::sin(timePassed*speed) * -glm::sin(movementAzimuthalAngle) +
		movementCircleCenter.z;
	return position;
}

void EnemyShip::reduceHealthPoints(GLint reducedHealthPoints)
{
	healthPoints -= reducedHealthPoints;
	if (healthPoints < 0) healthPoints = 0;
}

void EnemyShip::destroy()
{
	this->inUse = false;
	this->healthPoints = this->maxHealthPoints;
	this->timePassed = 0.0f;
	this->translateVector = spawnPoint;
	this->movementState = EnemyShip::MOVING_TO_POSITION;
}

void EnemyShip::shootMissile()
{
	Missile* missile = getMissileFromPool();
	PointLight * pointLight = lightManager->createPointLight(
		translateVector + loadingMissileOffset,
		0.01f, 0.0014f, 0.000007f,
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.5f, 0.5f, 0.5f)
		);
	missile->init(translateVector + loadingMissileOffset, pointLight, shootingSpeed, glm::normalize(shootingTargetPosition - translateVector + loadingMissileOffset));
	// soundSystem->playSound(soundSystem->enemyShipShoot,
	// 	{ translateVector.x, translateVector.y, translateVector.z });
}

Missile* EnemyShip::getMissileFromPool()
{
	for (Missile* missile : *missilePool)
	{
		if (!missile->inUse)
		{
			missile->inUse = true;
			return missile;
		}
	}
	return nullptr;
}