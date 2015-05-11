#pragma once

#include "Matrix.h"
#include "Vector.h"

class Entity {
public:
	Entity();
	Entity(float x, float y, float z);
	Entity(float x, float y, float z, bool isBillboard);
	Entity(const Entity& rhs);
	Entity& operator=(const Entity& rhs);

	void buildMatrix();
	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	virtual void render() = 0;

	Matrix matrix;
	Vector position, scale, rotation;
	bool remove, isBillboard;
};