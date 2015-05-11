#pragma once

#include "Billboard.h"

class Enemy : public Billboard {
public:
	Enemy(float x, float y, float z, const Sprite &sprite);
	Enemy(const Enemy &rhs);
	
	int hp;
	float shootTime;
	Vector vertices[4];
};