#ifndef _TEXT_H_
#define _TEXT_H_

#include "Renderable.h"
#include "Shaders.h"
#include "Screen.h"

#include <GLFW/glfw3.h>

/*
Class representing 2d text.
Use "/" to make new line while setting text value.
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

	Text();
	virtual ~Text();

	void init(Screen * screen);
	void setTextValue(std::string textValue);
	void render() {};
	void render(int posX, int posY, int size);
	void update(float deltaTime);
	void destroy();
};

#endif
