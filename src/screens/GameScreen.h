#ifndef _GAMESCREEN_H_
#define _GAMESCREEN_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "World.h"
#include "WorldRenderer.h"

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
#include "SoundSystem.h"

#include <map>

/*
Main game class
*/
class GameScreen : public Screen
{
private:
	std::unique_ptr<World> world;
	std::unique_ptr<WorldRenderer> renderer;

	Rectangle crosshairs;
	float crosshairsSize;

	// Texts
	Text text;
	Text forceShieldHPtext;
	Text gameFinishedText;

	// Helper booleans
	GLboolean showingInstructions;
	GLboolean showingDebugInfo;
	GLboolean cameraCannonView;

	int windowWidth, windowHeight;

	// Helper map for handling key-down events for GLFW
	std::map<int, GLboolean> heldKeysMap;

	// Camera
	Camera camera;

	ShaderProgram textShaderProgram;

public:

	GameScreen(GLFWwindow * w): Screen(w) { }
	~GameScreen();

	void init();
	void render();
	void update(GLfloat deltaTime);
	void release();

	void updateCamera(GLfloat deltaTime);

	// Input
	void checkGameplayInputs(GLfloat deltaTime);
	void checkMenuInputs();
};

#endif