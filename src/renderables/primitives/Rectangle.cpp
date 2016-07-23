#include "Rectangle.h"


Rectangle::Rectangle()
{
}


Rectangle::~Rectangle()
{
}

void Rectangle::init(Screen * screen, std::string textureName)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &uvVBO);

	texture.loadTexture2D("data/textures/" + textureName, true);

}
void Rectangle::render(float posX, float posY, float width, float height)
{
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;

	glm::vec2 vertexUpLeft = glm::vec2(posX, posY + height);
	glm::vec2 vertexUpRight = glm::vec2(posX + width, posY + height);
	glm::vec2 vertexDownRight = glm::vec2(posX + width, posY);
	glm::vec2 vertexDownLeft = glm::vec2(posX, posY);

	vertices.push_back(vertexUpLeft);
	vertices.push_back(vertexDownLeft);
	vertices.push_back(vertexUpRight);
	vertices.push_back(vertexDownRight);
	vertices.push_back(vertexUpRight);
	vertices.push_back(vertexDownLeft);

	glm::vec2 uvUpLeft = glm::vec2(0.0f, 1.0f);
	glm::vec2 uvUpRight = glm::vec2(1.0f, 1.0f);
	glm::vec2 uvDownRight = glm::vec2(1.0f, 0.0f);
	glm::vec2 uvDownLeft = glm::vec2(0.0f, 0.0f);

	UVs.push_back(uvUpLeft);
	UVs.push_back(uvDownLeft);
	UVs.push_back(uvUpRight);
	UVs.push_back(uvDownRight);
	UVs.push_back(uvUpRight);
	UVs.push_back(uvDownLeft);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	texture.bindTexture();

	glBindVertexArray(VAO);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glDisable(GL_BLEND);
}
void Rectangle::update(float deltaTime)
{

}
void Rectangle::destroy()
{

}