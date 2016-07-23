#ifndef _RECTANGLE_H_
#define _RECTANGLE_H_

#include "Renderable.h"

class Rectangle : public Renderable
{
public:
	Rectangle();
	~Rectangle();

	void init(Screen*) {};
	void init(Screen* screen, std::string textureName);
	void render();
	void render(float posX, float posY, float width, float height);
	void update(float deltaTime);
	void destroy();
};

#endif