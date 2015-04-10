#pragma once

#include <cmath>
#include "Entity.h"
#include "Vector.h"

extern const float FIXED_TIMESTEP;

class Bullet : public Entity {
public:
	static const float VELOCITY;
	static const float TIME_ALIVE;

	Bullet();
	Bullet(float x, float y, float rotation, const Vector& playerVelocity);

	void fixedUpdate();
	void render();

	Vector velocity;
	float timeAlive;
};