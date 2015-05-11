#pragma once

#include "Vector.h"
#include <vector>
#include <math.h>
#include <SDL_opengl.h>
#include "Constants.h"

class BloodParticle {
public:
	BloodParticle(Vector position, Vector velocity, float maxLifeTime);

	Vector position;
	Vector velocity;
	float lifeTime;
	float maxLifeTime;
};

class BloodEmitter {
public:
	BloodEmitter();
	BloodEmitter(Vector position);
	BloodEmitter(const BloodEmitter& rhs);

	void fixedUpdate();
	void render();
	float lerp(float v0, float v1, float t);

	Vector position;
	std::vector<BloodParticle> particles;
	bool remove;
};