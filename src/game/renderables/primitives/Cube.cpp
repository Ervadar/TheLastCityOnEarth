#include "Cube.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Screen.h"
#include "GameScreen.h"

Cube::Cube(glm::vec4 color, glm::vec3 translateVector, glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY, glm::vec3 rotateAxisX, glm::vec3 rotateAxisY, std::string texName, std::string topTexName, float texRepeatX, float texRepeatY)
{
	this->color = color;
	this->texName = texName;
	this->topTexName = topTexName;
	this->texRepeatX = texRepeatX;
	this->texRepeatY = texRepeatY;

	Renderable::init(scaleVector, rotateAngleX, rotateAngleY, rotateAxisX, rotateAxisY, translateVector);
}

Cube::~Cube()
{
	destroy();
}

void Cube::init(Screen * screen)
{
	this->screen = screen;

	glm::vec3 cubeVertices[] = 
	{
		// Front face
		glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f),
		// Back face
		glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f),
		// Left face
		glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, 0.5f),
		// Right face
		glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
		// Top face
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 
		// Bottom face
		glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
	};

	glm::vec2 cubeTexCoords[] = 
	{
		glm::vec2(0.0f, texRepeatY), glm::vec2(texRepeatX, texRepeatY), glm::vec2(texRepeatX, 0.0f), glm::vec2(texRepeatX, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, texRepeatY),
		glm::vec2(0.0f, texRepeatY), glm::vec2(texRepeatX, texRepeatY), glm::vec2(texRepeatX, 0.0f), glm::vec2(texRepeatX, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, texRepeatY),
		glm::vec2(0.0f, texRepeatY), glm::vec2(texRepeatX, texRepeatY), glm::vec2(texRepeatX, 0.0f), glm::vec2(texRepeatX, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, texRepeatY),
		glm::vec2(0.0f, texRepeatY), glm::vec2(texRepeatX, texRepeatY), glm::vec2(texRepeatX, 0.0f), glm::vec2(texRepeatX, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, texRepeatY),
		glm::vec2(0.0f, texRepeatY), glm::vec2(texRepeatX, texRepeatY), glm::vec2(texRepeatX, 0.0f), glm::vec2(texRepeatX, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, texRepeatY),
		glm::vec2(0.0f, texRepeatY), glm::vec2(texRepeatX, texRepeatY), glm::vec2(texRepeatX, 0.0f), glm::vec2(texRepeatX, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, texRepeatY),
	};

	glm::vec3 cubeNormals[] =
	{
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
	};

	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &uvVBO);
	glGenBuffers(1, &normalVBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Load textures
	texture.loadTexture2D("data/textures/" + texName, true);
	texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	texture.setSamplerParameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

	textureTop.loadTexture2D("data/textures/" + topTexName, true);
	textureTop.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	textureTop.setSamplerParameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

	stableModelMatrix = glm::scale(glm::mat4(1.0f), scaleVector);
}

void Cube::render(ShaderProgram & shaderProgram)
{
	glBindVertexArray(VAO);

	// Texture binding and drawing
	texture.bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 24);

	// Górna œciana
	textureTop.bindTexture();
	glDrawArrays(GL_TRIANGLES, 24, 6);
}

void Cube::update(float deltaTime)
{
}

void Cube::destroy()
{
}