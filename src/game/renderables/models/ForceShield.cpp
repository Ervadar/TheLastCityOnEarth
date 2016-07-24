#include "ForceShield.h"

#include <glm/gtc/constants.hpp>

#include <iostream>
#include <fstream>


ForceShield::ForceShield()
{
}

ForceShield::~ForceShield()
{
	destroy();
}

void ForceShield::init(Screen * screen)
{
	this->screen = screen;
	this->maxHealthPoints = 2000;
	this->healthPoints = maxHealthPoints;
	this->healthPerSecond = 1;

	radius = 850.0f;
	Renderable::init(glm::vec3(radius), 0.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-350.0f, -100.0f, -350.0f));

	GLfloat M_PI = glm::pi<GLfloat>();
	GLfloat M_PI_2 = glm::half_pi<GLfloat>();

	GLfloat x, y, z;

	int rings = 100;
	int sectors = 100;

	int r, s;

	GLfloat R = 1.0f/(GLfloat)(rings-1);
    GLfloat S = 1.0f/(GLfloat)(sectors-1);

	std::vector<GLfloat> vertices;
    std::vector<GLfloat> textureCoords;
	std::vector<GLfloat> normals;
    std::vector<GLushort> indicesOut;

    vertices.resize(rings * sectors * 3);
    textureCoords.resize(rings * sectors * 2);
	normals.resize(rings * sectors * 3);
    std::vector<GLfloat>::iterator v = vertices.begin();
    std::vector<GLfloat>::iterator t = textureCoords.begin();
	std::vector<GLfloat>::iterator n = normals.begin();
    for(r = 0; r < rings; r++)
	{
		for(s = 0; s < sectors; s++)
		{
			x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
			y = sin( -M_PI_2 + M_PI * r * R );
			z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

			*t++ = s*S*10;
			*t++ = r*R*10;

			*v++ = x;
			*v++ = y;
			*v++ = z;

			*n++ = x;
            *n++ = y;
            *n++ = z;
		}
	}
    indicesOut.resize(rings * sectors * 6);
    std::vector<GLushort>::iterator i = indicesOut.begin();
    for(r = rings/2; r < rings-1; ++r)
	{
		for(s = 0; s < sectors-1; ++s)
		{
                *i++ = (r+1) * sectors + s;
				*i++ = (r+1) * sectors + (s+1);
				*i++ = r * sectors + (s+1);

				*i++ = r * sectors + (s+1);
				*i++ = r * sectors + s;
				*i++ = (r+1) * sectors + s;				
        }
	}
    std::vector<GLushort> indicesIn;
    indicesIn.resize(rings * sectors * 3);
    std::vector<GLushort>::iterator testi = indicesIn.begin();
    for(r = rings/2; r < rings-1; ++r)
	{
		for(s = 0; s < sectors-1; ++s)
		{
				*testi++ = r * sectors + (s+1);
				*testi++ = (r+1) * sectors + (s+1);
				*testi++ = (r+1) * sectors + s;
				
				*testi++ = (r+1) * sectors + s;
				*testi++ = r * sectors + s;
				*testi++ = r * sectors + (s+1);
        }
	}

	// Store number of indices (indicesOut.size() == indicesIn.size())
	indicesSize = indicesIn.size();

	rotateAngle = 0.001f;
	color = glm::vec4(0.5f, 0.82f, 0.9f, 0.4f);

	//stableModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-350.0f, -100.0f, -350.0f));
	stableModelMatrix = glm::scale(stableModelMatrix, scaleVector);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &uvVBO);
	glGenBuffers(1, &normalVBO);
	glGenBuffers(1, &elementVBO);
	glGenBuffers(1, &innerElementVBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), &textureCoords[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Outer force field
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesOut.size() * sizeof(GLushort), &indicesOut[0], GL_STATIC_DRAW);
	
	// Inner force field (NORMALS THE SAME AS OUTER!)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, innerElementVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesIn.size() * sizeof(GLushort), &indicesIn[0], GL_STATIC_DRAW);
	
	// Load textures
	texture.loadTexture2D("data/textures/hdfield.jpg", true);
	texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	texture.setSamplerParameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
}

void ForceShield::render(ShaderProgram & shaderProgram)
{
	texture.bindTexture();
	
	glEnable(GL_BLEND); 
	glDepthMask(0); // Disable writing to depth buffer

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, innerElementVBO);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_SHORT, 0);
	
	glDisable(GL_BLEND); 
	glDepthMask(1); // Re-enable writing to depth buffer
}

void ForceShield::update(float deltaTime)
{
	static float timePassed = 0.0f;
	timePassed += deltaTime;

	rotateAngleY -= deltaTime*2.0f;

	if (timePassed > 1.0f)
	{
		timePassed = 0.0f;
		if (healthPoints < maxHealthPoints)
			healthPoints += healthPerSecond;
	}
}

void ForceShield::destroy()
{
}

void ForceShield::reduceHealthPoints(GLint reducedHealthPoints)
{
	healthPoints -= reducedHealthPoints;
	if (healthPoints < 0) healthPoints = 0;
}