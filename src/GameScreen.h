#ifndef _GAMESCREEN_H_
#define _GAMESCREEN_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Screen.h"
#include "Renderable.h"
#include "Terrain.h"
#include "Cube.h"
#include "Cannon.h"
#include "ForceShield.h"
#include "Skybox.h"
#include "Text.h"
#include "EnemyShip.h"
#include "Missile.h"
#include "Explosion.h"
#include "Rectangle.h"

#include "ShadowMapper.h"
#include "Lighting.h"

#include <map>

/*
Main game class
*/
class GameScreen : public Screen
{
protected:
	Camera camera, depthCamera;
	// SoundSystem soundSystem;
private:
	// Spheres inside which lies a point, which will be the center of circural enemy ship movement
	std::vector<BoundingSphere> enemyShipDestinationSpheres;
	glm::vec3 enemyShipSpawnPoint;

	// Object pools
	static const GLuint ENEMY_SHIP_POOL_SIZE = 20;
	std::vector<std::unique_ptr<EnemyShip>> enemyShips;

	static const GLuint CANNON_MISSILES_POOL_SIZE = 15;
	std::vector<Missile*> cannonMissiles;

	static const GLuint ENEMY_SHIP_MISSILES_POOL_SIZE = 40;
	std::vector<Missile*> enemyShipMissiles;

	static const GLuint EXPLOSION_POOL_SIZE = 45;
	std::vector<Explosion> explosions;

	// Objects
	std::vector<Renderable*> staticObjects;

	Skybox skybox;

	std::unique_ptr<ForceShield> forceShield;
	std::unique_ptr<Cannon> cannon;
	Terrain terrain;
	Rectangle crosshairs;
	float crosshairsSize;

	// Texts
	Text text;
	Text forceShieldHPtext;
	Text gameFinishedTexts[4];
	
	ShadowMapper shadowMapper;
	LightManager lightManager;

	// Helper booleans
	GLboolean showingInstructions;
	GLboolean showingDebugInfo;
	GLboolean gamePaused;
	GLboolean gameFinished;
	GLboolean cameraCannonView;

	// Timers
	GLfloat timePassed;
	GLfloat timePassedFromLastShipSpawn;

	GLfloat maxEnemyShipSpawnTime;
	GLfloat minEnemyShipSpawnTime;
	GLfloat enemyShipSpawnTime;

	// Shader programs
	ShaderProgram shaderProgram, skyboxShaderProgram, terrainShaderProgram, 
		textShaderProgram, screenShaderProgram, colorShaderProgram;

	float near, far;
	int windowWidth, windowHeight;

	// Helper map for handling key-down events for GLFW
	std::map<int, GLboolean> heldKeysMap;

public:
	// Functions for setting shader uniforms
	typedef void (GameScreen::*matrixUniformFunction)(Renderable*, Camera&, ShaderProgram&);

	// Directional light - needs to be taken to Lighting class
	glm::vec3 sunDirection;
	glm::vec3 sunLightColor;
	GLfloat sunLightAmbientIntensity;
	glm::vec3 sunRotateAxisX;
	float sunRotateAngleX, sunRotateAngleY, sunSpeed;


	GameScreen(GLFWwindow * w): Screen(w) { }
	~GameScreen();

	void render(Renderable * object, Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction);
	void renderObjects(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction);
	void renderPointLights(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction);
	void renderObjectsCastingShadows(Camera & camera, ShaderProgram & shaderProgram, matrixUniformFunction matrixUniformFunction);

	void init();
	void render();
	void update(GLfloat deltaTime);
	void release();

	void updateCamera(GLfloat deltaTime);
	void updateDepthCamera(GLfloat deltaTime);

	// Input
	void checkGameplayInputs(GLfloat deltaTime);
	void checkMenuInputs();

	// Collisions
	void checkCollisions();

	void checkMissileBoundaryCollisions(std::vector<Missile*>& missiles);
	void checkMissileEnemyShipCollisions(std::vector<Missile*>& missiles);
	void checkMissileForceShieldCollisions(std::vector<Missile*>& missiles);
	void checkMissileTerrainCollisions(std::vector<Missile*>& missiles);

	bool missileCollidesWithObject(Missile* missile, Renderable* object);

	// Game functions
	void spawnEnemyShip();
	void spawnExplosion(glm::vec3 position);
	void spawnMissile(Missile * missile);
	void shootCannonMissile();

	void restartGame();

	// Matrix and Uniform functions
	void setMainShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
	void setTerrainShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
	void setSkyboxShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
	void setDepthShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
	void setColorShaderMatricesAndUniforms(Renderable * renderable, Camera & camera, ShaderProgram & shaderProgram);
};

#endif