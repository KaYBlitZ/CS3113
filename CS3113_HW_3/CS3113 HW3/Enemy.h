#pragma once

#include "Entity.h"
#include "SheetSprite.h"
#include "SpaceInvaderGame.h"
#include <cstdlib>

extern const int ENEMY_POINTS;
extern float ENEMY_MOVE_X;
extern const float ENEMY_MOVE_Y;

class SpaceInvaderGame;

class Enemy : public Entity {
public:
	Enemy(float x, float y, SheetSprite sprite);

	void update(float& elapsed);
	void update(float& elapsed, bool& isMoveRight, SpaceInvaderGame* game);
	void move(float x, float y);
private:
	bool isMoveRight;
	float timer;
};