#pragma once

#include "Entity.h"
#include <cmath>

extern const float FIXED_TIMESTEP;

enum AsteroidSize {
	LARGE,
	MEDIUM,
	SMALL
};

class Asteroid : public Entity {
public:
	Asteroid();
	Asteroid(float x, float y, float rotation, float velocity, AsteroidSize size);

	virtual ~Asteroid();
	Asteroid(const Asteroid& rhs);
	Asteroid& operator=(const Asteroid& rhs);

	void fixedUpdate();
	void render();

	float velocity;
	AsteroidSize size;
	GLfloat* shape;
	int numPoints;
};