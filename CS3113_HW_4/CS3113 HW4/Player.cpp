#include "Player.h"

using namespace std;

const float Player::PLAYER_FRIC_X = 0.5f;
const float Player::PLAYER_FRIC_Y = 0.5f;
const float Player::X_ACCEL = 0.1f;

Player::Player() : collideLeft(false), collideTop(false), collideRight(false), collideBottom(false) {}
Player::Player(float x, float y, SheetSprite sprite) : Entity(x, y, sprite.texWidth, sprite.texHeight, sprite), collideLeft(false), collideTop(false), collideRight(false), collideBottom(false) {}

Player& Player::operator =(const Player& rhs) {
	if (&rhs != this) {
		Entity::operator=(rhs);
		collideLeft = rhs.collideLeft;
		collideTop = rhs.collideTop;
		collideRight = rhs.collideRight;
		collideBottom = rhs.collideBottom;
	}
	return *this;
}