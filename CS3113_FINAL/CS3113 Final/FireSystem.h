#pragma once

#include <vector>
#include <math.h>
#include <SDL_opengl.h>
#include "Constants.h"
#include "Vector.h"
#include "Sprite.h"
#include "Matrix.h"
#include "Billboard.h"

class FireParticle {
public:
	FireParticle(Vector position, Vector velocity, float maxLifeTime);

	Vector position;
	Vector velocity;
	float lifeTime;
	float alpha;
	float maxLifeTime;
};

class FireEmitter : public Billboard {
public:
	FireEmitter(Vector position, float width, float height, GLuint fire);

	void fixedUpdate();
	void render();
	float lerp(float v0, float v1, float t);

	float width, height;
	GLuint fire;
	std::vector<FireParticle> particles;
};