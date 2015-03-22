#pragma once

#include "SheetSprite.h"

class Entity {
public:
	Entity();
	Entity(float x, float y, float width, float height, SheetSprite sprite);
	Entity(const Entity& rhs);

	Entity& operator=(const Entity& rhs);

	virtual void render();

	float getScaledWidth();
	float getScaledHeight();

	float width, height;
	float x, y, xVel, yVel, xAccel, yAccel;
	bool remove;
protected:
	SheetSprite sprite;
};