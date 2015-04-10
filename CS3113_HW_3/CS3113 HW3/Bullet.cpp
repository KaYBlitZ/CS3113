#include "Bullet.h"

const float BULLET_VEL = 1.0f;

using namespace std;

Bullet::Bullet(float x, float y, SheetSprite sprite, BulletType type) : Entity(x, y, sprite), type(type) {}

void Bullet::update(float& elapsed) {
	switch (type) {
	case PLAYER:
		y += BULLET_VEL * elapsed;
		if (y > 1.0f) remove = true;
		break;
	case ENEMY:
		y -= BULLET_VEL * elapsed;
		if (y < -1.0f) remove = true;
		break;
	}
}

bool Bullet::isColliding(Entity* entity) {
	// since everything is centered at x, y...
	float x = this->x - (sprite.getWidth() * sprite.getScale()) / 2;
	float y = this->y - (sprite.getHeight() * sprite.getScale()) / 2;
	float eX = entity->getX() - (entity->getWidth() * entity->getScale()) / 2;
	float eY = entity->getY() - (entity->getHeight() * entity->getScale()) / 2;

	if (x + sprite.getWidth() * sprite.getScale() < eX ||
		x > eX + entity->getWidth() * entity->getScale() ||
		y + sprite.getHeight() * sprite.getScale() < eY ||
		y > eY + entity->getHeight() * entity->getScale())
	{
		return false;
	}
		return true;
}

BulletType Bullet::getType() {
	return type;
}