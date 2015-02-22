#pragma once

#include "SheetSprite.h"
#include "Entity.h"

const static float BULLET_VEL = 1.0f;

enum BulletType {
	PLAYER,
	ENEMY
};

class Bullet : public Entity {
public:
	Bullet(float x, float y, SheetSprite sprite, BulletType type);

	void update(float& elapsed);

	bool isColliding(Entity* entity);
	BulletType getType();
private:
	BulletType type;
};