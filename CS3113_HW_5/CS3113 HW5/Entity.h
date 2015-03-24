#pragma once

#include "SheetSprite.h"

class Entity {
public:
	Entity();
	Entity(float x, float y, SheetSprite sprite);
	Entity(const Entity& rhs);

	Entity& operator=(const Entity& rhs);

	virtual void render();

	float getWidth();
	float getHeight();

	float x, y, xVel, yVel, xAccel, yAccel;
	bool remove;
protected:
	SheetSprite sprite;
};