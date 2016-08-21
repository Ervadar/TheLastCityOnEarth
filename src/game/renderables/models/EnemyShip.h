#ifndef _ENEMYSHIP_H_
#define _ENEMYSHIP_H_
#include <random>
#include "Renderable.h"
#include "Sphere.h"
#include "Cube.h"
#include "Missile.h"
#include "SoundSystem.h"

class EnemyShip : public Renderable
{
private:
	glm::vec3 movementCircleCenter;
	GLfloat movementCircleRadius;
	GLfloat movementPolarAngle;
	GLfloat movementAzimuthalAngle;
	glm::vec3 movementStartingPosition;

	glm::vec3 movementToPositionDirection;

	glm::vec3 spawnPoint;

	GLfloat speed;
	GLfloat rotationSpeed;

	float timePassed;
	float shootingTimer;

public:
	enum class MovementState{ MOVING_TO_POSITION, MOVING_AROUND_POINT };
	MovementState movementState;
	enum class MissileState{ LOADING, READY };
	MissileState missileState;

	GLint maxHealthPoints;
	GLint healthPoints;

	Sphere * debugSphere;
	std::vector<Cube*> debugBoundingBoxes;

	glm::vec3 shootingTargetPosition;

	GLfloat timeBetweenShoots;
	GLfloat shootingSpeed;

	std::vector<std::unique_ptr<Missile>>* missilePool;

	glm::vec3 loadingMissileOffset;

	LightManager * lightManager;

	EnemyShip(glm::vec3 position, std::vector<std::unique_ptr<Missile>>* missilePool);
	~EnemyShip();

	void update(float deltaTime);
	void destroy();

	void calculateCircularMovement(std::vector<BoundingSphere>& enemyShipDestinationSpheres);

	glm::vec3 getMovementCirclePosition(GLfloat timePassed);

	void reduceHealthPoints(GLint reducedHealthPoints);
	void shootMissile();
	Missile* getMissileFromPool();
};

#endif