#include "World.h"

extern std::mt19937 rng;

void World::init()
{
	gamePaused = false;
	gameFinished = false;

	// Sun (directional light)
	sunDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	sunLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	sunLightAmbientIntensity = 0.5f;
	sunRotateAngleX = 47.0f;
	sunRotateAngleY = 315.0f;
	sunSpeed = 50.0f;

	// Creating cannon
	cannon = std::move(std::unique_ptr<Cannon>(new Cannon()));

	// Creating the city
	float basePosX = 50.0f;
	float basePosZ = 50.0f;
	float width = 100.0f;
	float height = 400.0f;
	float length = 100.0f;
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
		{
			staticObjects.push_back(std::move(std::unique_ptr<Cube>(new Cube(
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
				))));
		}

	// Cannon base
	int i = 0, j = 0;
	staticObjects.push_back(std::move(std::unique_ptr<Cube>(new Cube(
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
		))));

	// City force shield
	forceShield = std::move(std::unique_ptr<ForceShield>(new ForceShield()));
	forceShield->castingShadow = false;

	// Initializing objects
	skybox.init();
	terrain.init();
	cannon->init();
	for (auto& object : staticObjects)
	{
		object->init();
	}
	enemyShipSpawnPoint = glm::vec3(0.0f, 4500.0f, 0.0f);

	for (GLuint i = 0; i < ENEMY_SHIP_POOL_SIZE; ++i)
	{
		std::unique_ptr<EnemyShip> enemyShip(new EnemyShip(enemyShipSpawnPoint, &enemyShipMissiles));
		enemyShip->lightManager = &lightManager;
		enemyShip->shootingTargetPosition = forceShield->translateVector;
		enemyShips.push_back(std::move(enemyShip));
	}
	forceShield->init();

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

	// Init cannon missile pool
	for (GLuint i = 0; i < CANNON_MISSILES_POOL_SIZE; ++i)
	{
		cannonMissiles.push_back(std::move(std::unique_ptr<Missile>(new Missile(Missile::OWNER_CANNON))));
	}
	// Init enemy ship missile pool
	for (GLuint i = 0; i < ENEMY_SHIP_MISSILES_POOL_SIZE; ++i)
	{
		enemyShipMissiles.push_back(std::move(std::unique_ptr<Missile>(new Missile(Missile::OWNER_ENEMY_SHIP))));
	}

	// Game info
	this->timePassed = 0.0f;
	this->timePassedFromLastShipSpawn = 0.0f;

	this->maxEnemyShipSpawnTime = 6.0f;
	this->minEnemyShipSpawnTime = 1.5f;

	this->enemyShipSpawnTime = maxEnemyShipSpawnTime;

	lightManager.init();

	ParticleEffect::loadEffectFromFile("missile_explosion");
	ParticleEffect::loadEffectFromFile("ship_explosion");
	ParticleEffect::loadEffectFromFile("shield_hit");

	// Init explosion pool
	for (GLuint i = 0; i < SINGLE_EFFECT_TYPE_POOL_SIZE; ++i)
	{
		particleEffects["missile_explosion"].push_back(std::move(std::make_unique<ParticleEffect>("missile_explosion")));
	}
	for (GLuint i = 0; i < SINGLE_EFFECT_TYPE_POOL_SIZE; ++i)
	{
		particleEffects["ship_explosion"].push_back(std::move(std::make_unique<ParticleEffect>("ship_explosion")));
	}
	for (GLuint i = 0; i < SINGLE_EFFECT_TYPE_POOL_SIZE; ++i)
	{
		particleEffects["shield_hit"].push_back(std::move(std::make_unique<ParticleEffect>("shield_hit")));
	}
}

void World::update(GLfloat deltaTime)
{
	// Update sun
	sunRotateAxisX = glm::vec3(glm::rotate(glm::mat4(1.0f), sunRotateAngleY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	sunRotateAngleY = std::fmod(sunRotateAngleY + deltaTime*5.0f, 360.0f);
	sunDirection = glm::vec3(
		glm::rotate(glm::mat4(1.0f), sunRotateAngleX, sunRotateAxisX) *
		glm::rotate(glm::mat4(1.0f), sunRotateAngleY, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	for (auto& object : staticObjects)
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

	// Updating particle effects
	for (auto& mapPair : particleEffects)
	{
		auto& singleEffectPool = mapPair.second;
		for (GLuint i = 0; i < singleEffectPool.size(); ++i)
		{
			if (singleEffectPool[i]->inUse) singleEffectPool[i]->update(deltaTime, camera);
		}
	}

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
	}

	timePassed += deltaTime;
}

void World::release()
{
	staticObjects.clear();
	enemyShipMissiles.clear();
	cannonMissiles.clear();
	for (auto texture : Texture::loadedTextures) texture.releaseTexture();
	Texture::loadedTextures.erase(Texture::loadedTextures.begin(), Texture::loadedTextures.end());
	std::vector<Texture>().swap(Texture::loadedTextures);
}

void World::initCamera(GLFWwindow* window, int viewportWidth, int viewportHeight)
{
	camera.setPerspectiveMatrix(45.0f, (float)viewportWidth / (float)viewportHeight, 0.2f, 4000.0f);
	camera.init(
		window,
		glm::vec3(0, 27, 50),	// Camera position
		3.14f,	// Horizontal angle
		0.0f,	// Vertical angle
		300.0f,	// Camera speed
		0.01f	// Mouse speed
		);
}

// COLLISIONS
void World::checkCollisions()
{
	checkMissileBoundaryCollisions(cannonMissiles);
	checkMissileEnemyShipCollisions(cannonMissiles);
	checkMissileForceShieldCollisions(cannonMissiles);
	checkMissileTerrainCollisions(cannonMissiles);
	checkMissileForceShieldCollisions(enemyShipMissiles);
}

void World::checkMissileBoundaryCollisions(std::vector<std::unique_ptr<Missile>>& missiles)
{
	for (auto& missile : missiles)
	{
		if (!missile->inUse) continue;
		if (glm::abs(missile->translateVector.x) > 4000.0f || glm::abs(missile->translateVector.y) > 4000.0f || glm::abs(missile->translateVector.z) > 4000.0f)
		{
			spawnParticleEffect("missile_explosion", missile->translateVector);
			missile->destroy();
		}
	}
}
void World::checkMissileEnemyShipCollisions(std::vector<std::unique_ptr<Missile>>& missiles)
{
	for (auto& missile : missiles)
	{
		if (!missile->inUse) continue;
		for (auto& enemyShip : enemyShips)
		{
			if (!enemyShip->inUse) continue;
			if (missileCollidesWithObject(missile.get(), enemyShip.get()))
			{
				GLfloat hitDistanceFromCenter = glm::distance(missile->translateVector, enemyShip->translateVector);
				GLint missileStrength = missile->strength;
				enemyShip->reduceHealthPoints(missileStrength);
				if (enemyShip->healthPoints == 0)
				{
					spawnParticleEffect("ship_explosion", enemyShip->translateVector);
					enemyShip->destroy();
				}
				spawnParticleEffect("missile_explosion", missile->translateVector);
				SoundSystem::getInstance().playSound("data/sounds/enemyShipHit.wav", missile->translateVector);
				missile->destroy();
				break;
			}
		}
	}
}
void World::checkMissileForceShieldCollisions(std::vector<std::unique_ptr<Missile>>& missiles)
{
	for (auto& missile : missiles)
	{
		if (!missile->inUse) continue;
		if (missileCollidesWithObject(missile.get(), forceShield.get()))
		{
			spawnParticleEffect("shield_hit", missile->translateVector);
			forceShield->reduceHealthPoints(missile->strength);
			SoundSystem::getInstance().playSound("data/sounds/shieldHit.wav", missile->translateVector);
			missile->destroy();
		}
	}
}
void World::checkMissileTerrainCollisions(std::vector<std::unique_ptr<Missile>>& missiles)
{
	for (auto& missile : missiles)
	{
		if (!missile->inUse) continue;
		if (missile->translateVector.y < terrain.getTerrainHeight(missile->translateVector.x, missile->translateVector.z))
		{
			spawnParticleEffect("missile_explosion", missile->translateVector);
			SoundSystem::getInstance().playSound("data/sounds/enemyShipHit.wav", missile->translateVector);
			missile->destroy();
		}
	}
}

// Misisle-object collission

bool World::missileCollidesWithObject(Missile* missile, Renderable* object)
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

void World::spawnEnemyShip()
{
	for (GLuint i = 0; i < ENEMY_SHIP_POOL_SIZE; ++i)
	{
		if (!enemyShips[i]->inUse)
		{
			enemyShips[i]->translateVector = enemyShipSpawnPoint;
			std::uniform_real_distribution<GLfloat> enemyShipPositionOffset(-1100.0f, 1100.0f);
			enemyShips[i]->translateVector.x += enemyShipPositionOffset(rng);
			enemyShips[i]->translateVector.z += enemyShipPositionOffset(rng);
			enemyShips[i]->calculateCircularMovement(enemyShipDestinationSpheres);
			enemyShips[i]->movementState = EnemyShip::MovementState::MOVING_TO_POSITION;
			enemyShips[i]->inUse = true;
			return;
		}
	}
}

void World::spawnParticleEffect(std::string effectName, glm::vec3 position)
{
	auto& singleEffectPool = particleEffects[effectName];
	for (GLuint i = 0; i < singleEffectPool.size(); ++i)
	{
		if (!singleEffectPool[i]->inUse)
		{
			singleEffectPool[i]->translateVector = position;
			singleEffectPool[i]->inUse = true;
			singleEffectPool[i]->activate();
			return;
		}
	}
}

void World::shootCannonMissile()
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
		cannon->status = Cannon::Status::SHOOTING;
		SoundSystem::getInstance().playSound("data/sounds/cannonShoot.wav", cannon->translateVector);
		// soundSystem.playSound(soundSystem.cannonShootSound,
		// { cannon->translateVector.x, cannon->translateVector.y, cannon->translateVector.z }
		// );
		return;
	}
}

void World::reset()
{
	gameFinished = false;
	forceShield->healthPoints = forceShield->maxHealthPoints;
	for (auto& enemyShip : enemyShips) enemyShip->destroy();
	for (auto& cannonMissile : cannonMissiles) cannonMissile->inUse = false;
	for (auto& enemyShipMissile : enemyShipMissiles) enemyShipMissile->inUse = false;
	for (auto& mapPair : particleEffects)
	{
		auto& singleEffectPool = mapPair.second;
		for (GLuint i = 0; i < singleEffectPool.size(); ++i)
		{
			if (singleEffectPool[i]->inUse) singleEffectPool[i]->inUse = false;
		}
	}
	for (GLuint i = 0; i < lightManager.pointLights.size(); ++i) lightManager.pointLights[i]->inUse = false;
	timePassed = 0.0f;
	enemyShipSpawnTime = maxEnemyShipSpawnTime;
}