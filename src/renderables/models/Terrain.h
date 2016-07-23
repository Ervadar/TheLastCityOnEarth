#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#define NUMTERRAINSHADERS 2

#include "Renderable.h"
#include "Shaders.h"
#include "Texture.h"
#include "Screen.h"
#include "VertexBufferObject.h"

#include <GLFW/glfw3.h>

class Terrain : public Renderable
{
public:
	Texture textures[3];
	std::vector<std::vector<GLfloat>> terrainHeightValues;

	bool loaded; 
	bool shaderProgramLoaded; 
	int rows; 
	int cols; 

	glm::vec3 renderScale; 

	VertexBufferObject heightmapDataVBO; 
	VertexBufferObject heightmapIndicesVBO; 

	static ShaderProgram terrain; 
	static Shader terrainShaders[NUMTERRAINSHADERS];

public: 
	static bool loadTerrainShaderProgram(); 
	static void releaseTerrainShaderProgram(); 

	bool loadHeightMapFromImage(std::string imagePath); 
	void releaseHeightmap(); 

	void setRenderSize(float quadSize, float height); 
	void setRenderSize(float renderX, float height, float renderZ); 

	int getNumHeightmapRows(); 
	int getNumHeightmapCols(); 

	static ShaderProgram * getShaderProgram(); 

	Terrain();
	virtual ~Terrain();

	void init(Screen * screen);
	void render(ShaderProgram & shaderProgram);
	void update(float deltaTime);
	void destroy();

	GLfloat getTerrainHeight(GLfloat xCoordWorldspace, GLfloat zCoordWorldspace);
};

#endif