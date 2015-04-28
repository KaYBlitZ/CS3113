#pragma once

#include "Constants.h"
#include "Matrix.h"
#include "SheetSprite.h"

class Entity {
public:
	Entity();
	Entity(float x, float y);
	Entity(const Entity& rhs);
	Entity& operator=(const Entity& rhs);

	void createMatrix();
	virtual void fixedUpdate() = 0;
	virtual void render() = 0;

	Matrix matrix;
	float x, y, scaleX, scaleY, rotation;
	bool remove;
};