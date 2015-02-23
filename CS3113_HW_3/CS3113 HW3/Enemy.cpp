#include "Enemy.h"

const int ENEMY_POINTS = 10;
float ENEMY_MOVE_X = 0.1f;
const float ENEMY_MOVE_Y = 0.1f;

Enemy::Enemy(float x, float y, SheetSprite sprite) : Entity(x, y, sprite), isMoveRight(true), timer(0.0f) {}

void Enemy::update(float& elapsed) {}

void Enemy::update(float& elapsed, bool& isMoveRight, SpaceInvaderGame* game) {
	if (isMoveRight) {
		x += ENEMY_MOVE_X * elapsed;
	}
	else {
		x -= ENEMY_MOVE_X * elapsed;
	}

	if (rand() % 1000 == 0) { // 0.001% chance to shoot bullet
		game->shootEnemyBullet(x, y);
	}
}

void Enemy::move(float x, float y) {
	this->x += x;
	this->y += y;
}