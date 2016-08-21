#ifndef _CANNON_H_
#define _CANNON_H_

#include "Renderable.h"
#include "Sphere.h"
#include "Cylinder.h"

class Cannon : public Renderable
{
public:
	enum class Status{ READY, SHOOTING, LOADING };
	Status status;

	std::unique_ptr<Sphere> cannonSphere;
	std::unique_ptr<Cylinder> barrel;
	std::unique_ptr<Cylinder> movingBarrel;

	glm::vec3 cannonInitialDirectionVector;
	glm::vec3 cannonDirectionVector;

	float bulletSpawnOffset;
	float bulletSpeed;

	float cannonSpeed;
	float cannonRotY, cannonRotX;

	GLfloat M_PI;

	float movingAngle;

	float minHorizontalAngle = 0.0f;
	float maxHorizontalAngle = 360.0f;
	float minVerticalAngle = -8.0f;
	float maxVerticalAngle = 90.0f;

	float movingBarrelPos;
	float initialMovingBarrelPos;
	float shootingSpeed;
	float loadingSpeed;

	Cannon();
	virtual ~Cannon() {};

	void init();
	void update(float deltaTime);

	void updateRotation(GLfloat rotateAngleX, GLfloat rotateAngleY);
};

#endif