#include "Enemy.h"

Enemy::Enemy(float x, float y, float z, const Sprite &sprite) : Billboard(x, y, z, sprite), hp(100), shootTime(0.0f) {}

Enemy::Enemy(const Enemy &rhs) : Billboard(rhs), hp(100), shootTime(0.0f) {
	for (size_t i = 0; i < 4; i++) {
		vertices[i] = rhs.vertices[i];
	}
}