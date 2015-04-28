#pragma once

#include "Entity.h"

class Player : public Entity {
public:
	static const float PLAYER_FRIC_X, PLAYER_FRIC_Y, X_ACCEL;

	Player();
	Player(float x, float y, SheetSprite sprite);
	Player& operator =(const Player& rhs);
	void fixedUpdate();
	void render();

	bool collideLeft, collideTop, collideRight, collideBottom;
};