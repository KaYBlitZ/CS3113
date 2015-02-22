#pragma once

#include <SDL.h>
#include "SheetSprite.h"
#include "Entity.h"

const static float PLAYER_VEL = 0.5f;
const static float SHOOT_TIME = 0.75f;

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