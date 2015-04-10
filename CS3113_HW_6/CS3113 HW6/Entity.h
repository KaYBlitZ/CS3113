#pragma once

#define _USE_MATH_DEFINES

#include <SDL_opengl.h>
#include <cmath>
#include "Matrix.h"

class Entity {
public:
	static const float degToRadRatio;

	Entity();
	Entity(float x, float y, float rotation);
	Entity(float x, float y, float rotation, float radius);
	Entity(const Entity& rhs);
	Entity& operator=(const Entity& rhs);

	void createMatrix();
	virtual void fixedUpdate() = 0;
	virtual void render() = 0;

	Matrix matrix;
	float x, y, scaleX, scaleY, rotation, radius;
	bool remove;
};