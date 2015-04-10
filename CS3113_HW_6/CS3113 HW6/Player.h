#pragma once

#include "Entity.h"
#include "Vector.h"

extern const float FIXED_TIMESTEP;

class Player : public Entity {
public:
	static const float WIDTH, HEIGHT, RADIUS, ACCELERATION, DAMAGED_DURATION;

	// player is triangle

	Player();
	virtual ~Player();
	Player(float x, float y, float rotation);
	Player& operator=(const Player& rhs);

	void fixedUpdate();
	void render();

	bool isDamaged;
	float damagedTime;
	int damagedState;
	Vector velocity;
	GLfloat *shape;
};