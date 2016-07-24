#include "PointLight.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

PointLight::PointLight(Screen * screen)
{
	this->Renderable::init(glm::vec3(8.0f, 8.f, 8.0f), 0.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f));
	this->inUse = false;
	this->init(screen);
}

PointLight::~PointLight()
{
}

PointLight::PointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	Renderable::init(glm::vec3(8.0f, 8.f, 8.0f), 0.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), position);
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->ambient = ambient;
	this->color = glm::vec4(diffuse, 1.0f);	
	this->specular = specular;
	this->inUse = false;
}

void PointLight::init(Screen * screen)
{
	this->screen = screen;

	GLfloat M_PI = glm::pi<GLfloat>();
	GLfloat M_PI_2 = glm::half_pi<GLfloat>();

	GLfloat x, y, z;

	int rings = 20;
	int sectors = 20;

	int r, s;

	GLfloat R = 1.0f / (GLfloat)(rings - 1);
	GLfloat S = 1.0f / (GLfloat)(sectors - 1);

	std::vector<GLfloat> vertices;
	std::vector<GLushort> indices;

	vertices.resize(rings * sectors * 3);
	std::vector<GLfloat>::iterator v = vertices.begin();
	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			y = sin(-M_PI_2 + M_PI * r * R);
			z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			*v++ = x;
			*v++ = y;
			*v++ = z;
		}
	}
	indices.resize(rings * sectors * 6);
	std::vector<GLushort>::iterator i = indices.begin();
	for (r = 0; r < rings - 1; ++r)
	{
		for (s = 0; s < sectors - 1; ++s)
		{
			*i++ = (r + 1) * sectors + s;
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = r * sectors + (s + 1);

			*i++ = r * sectors + (s + 1);
			*i++ = r * sectors + s;
			*i++ = (r + 1) * sectors + s;
		}
	}

	// Store number of indices
	indicesSize = indices.size();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &elementVBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

	stableModelMatrix = glm::scale(glm::mat4(1.0f), scaleVector);
			
}

void PointLight::init(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->translateVector = position;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->ambient = ambient;
	this->color = glm::vec4(diffuse, 1.0f);
	this->specular = specular;
}

void PointLight::render(ShaderProgram & shaderProgram)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);

	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_SHORT, 0);
}
void PointLight::update(float deltaTime)
{
	//translateVector = translateVector + deltaTime * speed * direction;
}
void PointLight::destroy()
{

}