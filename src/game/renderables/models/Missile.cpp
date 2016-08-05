#include "Missile.h"


Missile::Missile(GLuint owner)
{
	this->owner = owner;
	this->inUse = false;
}


Missile::~Missile()
{
}

void Missile::init(glm::vec3 position, PointLight* pointLight, GLfloat speed, glm::vec3 direction)
{
	this->Renderable::init(position);
	this->pointLight = pointLight;
	this->radius = 8.0f;
	this->strength = 20;
	this->pointLight->scaleVector = glm::vec3(radius);
	this->speed = speed;
	this->direction = direction;
}

void Missile::update(float deltaTime)
{
	translateVector += deltaTime * speed * direction;
	pointLight->translateVector = translateVector;
}

void Missile::customRender(ShaderProgram & shaderProgram)
{

}

void Missile::destroy()
{
	inUse = false;
	pointLight->inUse = false;
}