#ifndef _CANNON_H_
#define _CANNON_H_

#include "Renderable.h"
#include "Sphere.h"
#include "Cylinder.h"

class Cannon : public Renderable
{
public:
	const int STATUS_READY = 0;
	const int STATUS_SHOOTING = 1;
	const int STATUS_LOADING = 2;

	Sphere * cannonSphere;
	Cylinder * barrel;
	Cylinder * movingBarrel;

	glm::vec3 cannonInitialDirectionVector;
	glm::vec3 cannonDirectionVector;

	float bulletSpawnOffset;
	float bulletSpeed;

	float cannonSpeed;
	float cannonRotY, cannonRotX;

	GLfloat M_PI;

	int cannonStatus;

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
	void render(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();

	void updateRotation(GLfloat rotateAngleX, GLfloat rotateAngleY);
};

#endif