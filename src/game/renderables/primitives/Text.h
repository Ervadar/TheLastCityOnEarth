#ifndef _TEXT_H_
#define _TEXT_H_

#include "Renderable.h"

#include <GLFW/glfw3.h>

/*
Class representing 2d text.
*/
class Text : public Renderable
{
private:
	GLuint alphabetTextureID;

public:
	std::string textValue;
	int posX;
	int posY;
	int size;

	unsigned int numberOfLines = 0;
	unsigned int textWidthInChars = 0;

	Text() {};
	virtual ~Text();

	void init();

	// Use "/" to make a new line
	void setTextValue(std::string textValue);

	void render() {};
	void render(int posX, int posY, int size);
	void update(float deltaTime);
	void destroy();

	void calculateTextSize();
};

#endif
