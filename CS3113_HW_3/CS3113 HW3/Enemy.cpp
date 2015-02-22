#include "Enemy.h"

Enemy::Enemy(float x, float y, SheetSprite sprite) : Entity(x, y, sprite), isMoveRight(true), timer(0.0f) {}

void Enemy::update(float& elapsed) {}

void Enemy::update(float& elapsed, SpaceInvaderGame* game) {
	if (rand() % 1000 == 0) { // 0.001% chance to shoot bullet
		game->shootEnemyBullet(x, y);
	}
}

void Enemy::move(float x, float y) {
	this->x += x;
	this->y += y;
}