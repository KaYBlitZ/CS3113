#pragma once

#include "Entity.h"

class Player : public Entity {
public:
	static const float PLAYER_FRIC_X, X_ACCEL;
	bool collideLeft, collideTop, collideRight, collideBottom;

	Player();
	Player(float x, float y, SheetSprite sprite);
	Player& operator =(const Player& rhs);
};