#pragma once

#include "SheetSprite.h"

class Entity {
public:
	Entity(float x, float y);
	Entity(float x, float y, SheetSprite sprite);

	Entity& operator=(const Entity& rhs);

	virtual void update(float& elapsed) = 0;
	void render();

	bool canRemove();
	void setRemove(bool remove);

	float getX();
	float getY();
	float getWidth();
	float getHeight();
	float getScale();
protected:
	float x, y;
	bool remove;
	SheetSprite sprite;
};