#include "Cylinder.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

Cylinder::Cylinder(glm::vec3 translateVector, glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY, glm::vec3 rotateAxisX, glm::vec3 rotateAxisY, std::string texName)
{
	this->texName = texName;

	Renderable::init(scaleVector, rotateAngleX, rotateAngleY, rotateAxisX, rotateAxisY, translateVector);
}


Cylinder::~Cylinder()
{
	destroy();
}

void Cylinder::init()
{
	GLfloat M_PI = glm::pi<GLfloat>();
	GLfloat M_PI_2 = glm::half_pi<GLfloat>();

	GLfloat theta, x, y, z;
	int sectors = 20;

	GLfloat S = 1.0f/(GLfloat)(sectors-1);

	std::vector<GLfloat> vertices;
    std::vector<GLfloat> textureCoords;
	std::vector<GLfloat> normals;
    std::vector<GLushort> indices;

	vertices.resize(3 * (sectors*2 + 2));
    textureCoords.resize(2 * (sectors*2 + 2));
	normals.resize(3 * (sectors*2 + 2));

	std::vector<GLfloat>::iterator v = vertices.begin();
    std::vector<GLfloat>::iterator t = textureCoords.begin();
	std::vector<GLfloat>::iterator n = normals.begin();

	// PODSTAWA 1
	for(int i = 0; i < sectors; ++i)
	{
		theta = 2*M_PI*i/sectors;
		x = (GLfloat)scaleVector.x*cosf(theta);
		y = (GLfloat)scaleVector.y*sinf(theta);
		z = (GLfloat)-scaleVector.z;

		*v++ = x;
		*v++ = y;
		*v++ = z;

		*t++ = 0.0f + i%2;
		*t++ = z*S;

		*n++ = x;
		*n++ = y;
		*n++ = 0.0f;
	}

	// PODSTAWA 2
	for(int i = 0; i < sectors; ++i)
	{
		theta = 2*M_PI*i/sectors;
		x = (GLfloat)scaleVector.x*cosf(theta);
		y = (GLfloat)scaleVector.y*sinf(theta);
		z = (GLfloat)scaleVector.z;

		*v++ = x;
		*v++ = y;
		*v++ = z;

		*t++ = 0.0f + i%2;
		*t++ = z*S;

		*n++ = x;
		*n++ = y;
		*n++ = 0.0f;
	}

	// SRODKI PODSTAW
	*v++ = 0.0f;
	*v++ = 0.0f;
	*v++ = -scaleVector.z;

	*t++ = 0.0f;
	*t++ = 0.5f;

	*n++ = 0.0f;
	*n++ = 0.0f;
	*n++ = -1.0f;

	*v++ = 0.0f;
	*v++ = 0.0f;
	*v++ = scaleVector.z;

	*t++ = 0.0f;
	*t++ = 0.5f;

	*n++ = 0.0f;
	*n++ = 0.0f;
	*n++ = 1.0f;

	indices.resize(6 * (sectors*2 + 2));
	std::vector<GLushort>::iterator i = indices.begin();

	for(int s = 0; s < sectors; ++s)
	{
		// BOKI
		*i++ = s;
		*i++ = (s+1)%sectors;
		*i++ = s + sectors;

		*i++ = (s+1)%sectors;
		*i++ = (s+1)%sectors + sectors;
		*i++ = s + sectors;

		// PODSTAWY
		*i++ = 2*sectors + 1;
		*i++ = s + sectors;
		*i++ = (s+1)%sectors + sectors;

		*i++ = 2*sectors;
		*i++ = (s+1)%sectors;
		*i++ = s;
	}

	// Store number of indices
	indicesSize = indices.size();
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &uvVBO);
	glGenBuffers(1, &normalVBO);
	glGenBuffers(1, &elementVBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), &textureCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
	
	// Load textures
	texture.loadTexture2D("data/textures/" + texName, true);
	texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	texture.setSamplerParameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

	stableModelMatrix = glm::scale(glm::mat4(1.0f), scaleVector);
}

void Cylinder::customRender(ShaderProgram & shaderProgram)
{
	glBindVertexArray(VAO);

	texture.bindTexture();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);

	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_SHORT, 0);
}

void Cylinder::update(float deltaTime)
{
}

void Cylinder::destroy()
{
}