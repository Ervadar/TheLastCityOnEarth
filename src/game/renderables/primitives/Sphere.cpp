#include "Sphere.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

Sphere::Sphere(glm::vec4 color, glm::vec3 translateVector, GLfloat radius, GLfloat rotateAngleX, GLfloat rotateAngleY, std::string texName)
{
	this->texName = texName;
	this->radius = radius;
	Renderable::init(glm::vec3(radius), rotateAngleX, rotateAngleY, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), translateVector);
	this->color = color;
	sphereSpeed = 0.0f;
}

Sphere::~Sphere()
{
}

void Sphere::init()
{
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
    std::vector<GLushort> indices;

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
    indices.resize(rings * sectors * 6);
    std::vector<GLushort>::iterator i = indices.begin();
    for(r = 0; r < rings-1; ++r)
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

	// Store number of indices
	indicesSize = indices.size();

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

void Sphere::render(ShaderProgram & shaderProgram)
{
	texture.bindTexture();
	if(color.a != 1.0f)
	{
		glEnable(GL_BLEND); 
		glDepthMask(0); // Disable writing to depth buffer
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);

	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_SHORT, 0);

	if(color.a != 1.0f)
	{
		glDisable(GL_BLEND); 
		glDepthMask(1); // Re-enable writing to depth buffer
	}
}

void Sphere::update(float deltaTime)
{
	translateVector = translateVector + deltaTime * sphereSpeed * direction;

	//modelMatrix = glm::translate(modelMatrix, translateVector);
	//modelMatrix = glm::rotate(modelMatrix, rotateAngleX, rotateAxis);
	//modelMatrix = glm::scale(modelMatrix, scaleVector);
	
}

void Sphere::destroy()
{
}