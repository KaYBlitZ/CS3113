#pragma once

#include "Entity.h"
#include "SheetSprite.h"
#include "SpaceInvaderGame.h"
#include <cstdlib>

const static int ENEMY_POINTS = 10;
static float ENEMY_MOVE_TIME = 1.5f;
const static float ENEMY_MOVE_X = 0.1f;
const static float ENEMY_MOVE_Y = 0.05f;

class SpaceInvaderGame;

class Enemy : public Entity {
public:
	Enemy(float x, float y, SheetSprite sprite);

	void update(float& elapsed);
	void update(float& elapsed, SpaceInvaderGame* game);
	void move(float x, float y);
private:
	bool isMoveRight;
	float timer;
};