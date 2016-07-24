#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_

#include "Shaders.h"
#include "Camera.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Screen.h"
#include "Texture.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Screen;

struct BoundingSphere
{
	BoundingSphere(){};
	BoundingSphere(glm::vec3 position, GLfloat radius) : position(position), radius(radius){};
	glm::vec3 position;
	GLfloat radius;
};

struct AABB
{
	AABB(){};
	AABB(glm::vec3 positionOffset, glm::vec3 scale) 
		: positionOffset(positionOffset), scale(scale){};
	glm::vec3 positionOffset;
	glm::vec3 scale;
	// C1.x - left-most face, C1.y - bottom-most face, C1.z - farthest face
	glm::vec3 getC1(glm::vec3 objectPosition) { return objectPosition - scale / 2.0f; }
	// C2.x - right-most face, C2.y - top-most face, C2.z - nearthest face
	glm::vec3 getC2(glm::vec3 objectPosition) { return objectPosition + scale / 2.0f; }
};


/*
Class representing renderable objects.
Class can be either inherited to create own type of renderable
or instantiated to load model from file (consisting of multiple meshes), ussing Assimp library (OBJ, etc.)
*/
class Renderable
{
protected:
	Screen * screen;

	GLuint VAO;
	GLuint vertexVBO;
	GLuint uvVBO;
	GLuint normalVBO;
	GLuint elementVBO;

	int indicesSize;

private:
	std::vector<Mesh> meshes;
	std::string directory;

public:
	std::vector<AABB> boundingBoxes;
	bool inUse;
	bool castingShadow;
	GLfloat radius;
	glm::vec3 direction;
	GLfloat speed;
	glm::vec4 color;

	glm::mat4 modelMatrix;
	glm::mat4 stableModelMatrix;
	glm::vec3 translateVector;
	glm::vec3 scaleVector;
	GLfloat rotateAngle;
	GLfloat rotateAngleX;
	GLfloat rotateAngleY;
	glm::vec3 rotateAxisX;
	glm::vec3 rotateAxisY;

	std::vector<Renderable*> subObjects;

	Texture texture;
	std::string texName;

public:
	Renderable(GLchar* modelPath, glm::vec3 translateVector, glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY, glm::vec3 rotateAxisX, glm::vec3 rotateAxisY);
	Renderable(GLchar* modelPath, glm::vec3 translateVector, glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY);
	Renderable(glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY, glm::vec3 rotateAxisX, glm::vec3 rotateAxisY, glm::vec3 translateVector);
	Renderable() {};
	virtual ~Renderable() {};

public:
	void init(glm::vec3 position);
	void init(GLchar* modelPath, glm::vec3 translateVector, glm::vec3 scaleVector);
	void init(glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY, glm::vec3 rotateAxisX, glm::vec3 rotateAxisY, glm::vec3 translateVector);
	virtual void init(Screen * screen){};
	virtual void render(ShaderProgram & shaderProgram);
	virtual void update(GLfloat deltaTime){};
	virtual void destroy(){};

	bool loadShaders(std::string vertexShaderName, std::string fragmentShaderName);
	bool hasSubObjects();
	void calculateMatricesAndUniforms(Camera & camera, ShaderProgram & shaderProgram);

	BoundingSphere getBoundingSphere();
	std::vector<AABB> & getBoundingBoxes();

private:
	void loadModel(std::string modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};

#endif
