#pragma once

#include "Entity.h"
#include "Constants.h"
#include "Vector.h"
#include "Ray.h"
#include "Sprite.h"
#include "SDL_opengl.h"

class Player : public Entity {
public:
	Player();
	Player(float x, float y, float z, const Sprite &gun);
	Player& operator=(const Player& rhs);
	void fixedUpdate();
	void render();
	void update();

	float height;
	int hp;
	Ray sight;
	Vector velocity;
	Sprite gun;
	bool canJump;
};