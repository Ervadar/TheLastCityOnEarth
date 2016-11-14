#include "Mesh.h"
#include <iostream>

Mesh::Mesh(std::vector<Vertex> & vertices, std::vector<GLuint> & indices, std::vector<Texture> & textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->init();
}

void Mesh::init()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, textureCoords));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
}

void Mesh::render(ShaderProgram shaderProgram)
{
	GLuint diffuseNum = 1;
	GLuint specularNum = 1;
	for (GLuint i = 0; i < this->textures.size(); ++i)
	{
		GLuint textureNum;
		std::string textureType = this->textures[i].type;
		if (textureType == "texture_diffuse")
			textureNum = diffuseNum++;
		else if (textureType == "texture_specular")
			textureNum = specularNum++;
		glActiveTexture(GL_TEXTURE0 + i);
		//std::cout << "material." + textureType + std::to_string(textureNum) << std::endl;
		shaderProgram.setUniform("material." + textureType + std::to_string(textureNum), (int)i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].texture);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	glBindVertexArray(this->VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glBindVertexArray(0);
}
