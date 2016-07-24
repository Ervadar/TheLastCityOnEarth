#ifndef _MESH_H_
#define _MESH_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shaders.h"
#include "Texture.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
};

/*
Class used by Renderable class while loading model (which can contain multiple meshes)
*/
class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> & vertices, std::vector<GLuint> & indices, std::vector<Texture> & textures);
	void render(ShaderProgram shaderProgram);
	~Mesh();
private:
	GLuint VAO, VBO, EBO;
	void init();
};

#endif _MESH_H_