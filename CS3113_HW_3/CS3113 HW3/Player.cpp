#include "Player.h"

Player::Player(float x, float y, SheetSprite sprite) : Entity(x, y, sprite), shoot(true), shootTime(0.0f) {}

Player::~Player() {}

void Player::update(float& elapsed) {}

void Player::update(float& elapsed, const Uint8* keys) {
	if (keys[SDL_SCANCODE_Z]) { // move left
		x -= PLAYER_VEL * elapsed;
	}
	else if (keys[SDL_SCANCODE_X]) { // move right
		x += PLAYER_VEL * elapsed;
	}

	float w = sprite.getWidth() / 2 * sprite.getScale();
	if (x + w > 1.0f) {
		x = 1.0f - w;
	}
	else if (x - w < -1.0f) {
		x = -1.0f + w;
	}

	if (!shoot) {
		shootTime += elapsed;

		if (shootTime > SHOOT_TIME) {
			shootTime = 0.0f;
			shoot = true;
		}
	}
}

bool Player::canShoot() {
	return shoot;
}

void Player::setShoot(bool shoot) {
	this->shoot = shoot;
}