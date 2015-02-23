#pragma once

#include <SDL.h>
#include "SheetSprite.h"
#include "Entity.h"

extern const float PLAYER_VEL;
extern const float SHOOT_TIME;

class Player : public Entity{
public:
	Player(float x, float y, SheetSprite sprite);
	~Player();

	void update(float& elapsed);
	void update(float& elapsed, const Uint8* keys);

	bool canShoot();
	void setShoot(bool shoot);
private:
	float shootTime;
	bool shoot;
};